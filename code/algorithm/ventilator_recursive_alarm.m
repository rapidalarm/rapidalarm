% Recursive envelope-tracking algorithm for Illinois RapidAlarm non-cycling
% alarm. The alarm is triggered after the ventilator stops cycling for a
% specified length of time or exceeds specified ranges of pressure or respiratory rate.
% It also tracks a moving average of maximum and
% minimum pressure and respiratory rate values for display.
%
% Requires Matlab R2018(b) or later
%
% Author: Ryan M. Corey, University of Illinois at Urbana-Champaign
% Date: April 13, 2020
%

%% Parameters

% Fixed parameters
params.DEFAULT_PIP = 25;
params.INITIAL_RR = 15;
params.NOMINAL_RATIO = 2.2;     % PIP-to-PEEP ratio under normal conditions
params.ALARM_DIFF = 3;          % High-to-low pressure difference to trigger alarm
params.ALARM_RATIO = 1.3;       % High-to-low pressure ratio to trigger alarm
params.ATTACK_TIME = 0.10;      % Time in seconds for attack envelope to change by 1/e
params.RELEASE_TIME = 15;       % Time constant used to compute release coefficient
params.DISPLAY_SMOOTHING = 0.5; % Smoothing coefficient per breath for display parameters

% User-adjustable parameters
params.ALARM_RR_MIN = 5;        % Alarm if breaths per minute below this level
params.ALARM_RR_MAX = 40;       % Alarm if breaths per minute above this level
params.MAX_ALARM = 40;          % Alarm if pressure goes above this level
params.MIN_ALARM = 2;           % Alarm if pressure drops below this level
params.ALARM_TIME = 15;         % Number of seconds after last breath when alarm should trigger

%% Pig trials 3/28

load test_data_20200328.mat;
state1 = recursive_envelope_loop(pig1,params);
figure(1); plot_alarm_state(state1,'Pig 1 (3/28/2020)');

state2 = recursive_envelope_loop(pig2,params);
figure(2); plot_alarm_state(state2,'Pig 2 (3/28/2020)');

%% Pig trials 3/31

load test_data_20200331.mat;
state3 = recursive_envelope_loop(pig3,params);
figure(3); plot_alarm_state(state3,'Pig 3 (3/31/2020)');

%% Lung simulator with microcontroller 4/1

load test_data_20200401.mat;
state4 = recursive_envelope_loop(cts10,params);
figure(4); plot_alarm_state(state4,'Microcontroller 10 Hz (4/01/2020)'); ylim([0 50]);

%% Lung simulator with microcontroller 4/3

load test_data_20200403.mat;
state5 = recursive_envelope_loop(cts100,params);
figure(5); plot_alarm_state(state5,'Microcontroller 100 Hz (4/03/2020)'); ylim([0 50]);

%%

function state = recursive_envelope_loop(sensor_in,params)
    % Envelope tracking coefficients
    sample_time = median(diff(second(sensor_in.Timestamp))); % Some animal testing data is nonuniformly sampled
    alarm_samples = params.ALARM_TIME/sample_time;
    release_samples = params.RELEASE_TIME/sample_time;
    attack_samples = params.ATTACK_TIME/sample_time;
    alarm_attack_coefficient = exp(-1/attack_samples);
    alarm_release_coefficient = ((params.ALARM_RATIO - 1)/(params.NOMINAL_RATIO - 1))^(1/release_samples);

    % Store algorithm state to visualize behavior
    state = timetable('RowTimes',sensor_in.Timestamp,'Size',[size(sensor_in,1),7],'VariableTypes',{'double','logical','double','double','double','double','double'},'VariableNames',{'Pressure','AlarmState','MaxPressure','MinPressure','RR','p_max','p_min'});

    % Initialize tracking variables
    p_max = params.DEFAULT_PIP;
    p_min = params.DEFAULT_PIP / params.NOMINAL_RATIO;
    time_since_last_max = 0;
    time_since_last_min = 0;
    last_max = p_max;
    last_min = p_min;
    breath_state = 0;
    time_since_last_breath = 0;
    avg_breath_time = floor(60/params.INITIAL_RR/sample_time);
    prev_breath_time = avg_breath_time;

    % Outputs
    alarm_state = false;
    display_max = p_max;
    display_min = p_min;
    display_rr = params.INITIAL_RR;

    % Main loop
    for t = 1:size(sensor_in,1)
        % Read pressure from sensor
        p = sensor_in.Pressure(t);
        time_since_last_breath = time_since_last_breath + 1;

        % Nonlinear peak tracking
        if p > p_max
            p_max = alarm_attack_coefficient*p_max + (1-alarm_attack_coefficient)*p;
            time_since_last_max = 0;
            last_max = p;

            % Entering increasing state
            if breath_state == 0
                breath_state = 1;

                % Update display min based on last min value
                display_min = params.DISPLAY_SMOOTHING*display_min + (1-params.DISPLAY_SMOOTHING)*last_min;
            end
        else
            p_max = alarm_release_coefficient*p_max + (1-alarm_release_coefficient)*p;
            time_since_last_max = time_since_last_max + 1;
        end
        if p < p_min
            p_min = alarm_attack_coefficient*p_min+ (1-alarm_attack_coefficient)*p;
            time_since_last_min = 0;
            last_min = p;

            % Entering decreasing state
            if breath_state == 1 && time_since_last_breath > 1
                breath_state = 0;

                % Update display max based on last max value
                display_max = params.DISPLAY_SMOOTHING*display_max + (1-params.DISPLAY_SMOOTHING)*last_max;

                % End of breath - track respiratory rate
                new_breath_time = time_since_last_breath-time_since_last_max;
                time_since_last_breath = time_since_last_max;

                % If breath is longer than max, set to max so that tracking
                % recovers when breathing restarts
                new_breath_time = min(new_breath_time,alarm_samples);

                % Breath time is smoothed using a filter with both feedback
                % and feedforward components because peaks can come at
                % different points in the breath cycle. Filter equation:
                % y[t] = a y[t-1] + (1-a)/2 x[t] + (1-a)/2 x[t-1]
                avg_breath_time = params.DISPLAY_SMOOTHING*avg_breath_time + (1-params.DISPLAY_SMOOTHING)/2*prev_breath_time + (1-params.DISPLAY_SMOOTHING)/2*new_breath_time;
                prev_breath_time = new_breath_time;
                display_rr = 60/(sample_time*avg_breath_time);
            end
        else
            p_min = alarm_release_coefficient*p_min + (1-alarm_release_coefficient)*p;
            time_since_last_min = time_since_last_min + 1;
        end

        % Check for alarm state
        alarm_state =  (p_max - p_min < params.ALARM_DIFF) | ... % Max and min too close
                       (p_max / p_min < params.ALARM_RATIO) | ... % Max and min too close
                       (time_since_last_max > alarm_samples) | ... % Too long since last max
                       (time_since_last_min > alarm_samples) | ... % Too long since last min
                       (p_max > params.MAX_ALARM) | ...    % Max pressure exceeded
                       (p_min < params.MIN_ALARM) | ...   % Min pressure exceeded
                       (display_rr > params.ALARM_RR_MAX) | ...            % Respiratory rate too fast
                       (display_rr < params.ALARM_RR_MIN);            % Respiratory rate too slow

        % Store state to analyze performance of algorithm
        state(t,:) = {p,alarm_state,display_max,display_min,display_rr,p_max,p_min};
    end
end

function plot_alarm_state(state,description)

    % Shade time intervals where alarm is active
    area(state.Time,state.AlarmState*50,'FaceColor',[0.5 0 0],'FaceAlpha',0.2,'LineStyle','none');

    % Plot pressure and max/min envelopes
    hold on;
    set(gca,'ColorOrderIndex',1);
    plot(state.Time,[state.Pressure,state.MaxPressure,state.MinPressure,state.RR,state.p_max,state.p_min]);
    hold off;

    % Scale and label plot
    ylim([0 30]);
    xlabel('Time'); ylabel('Pressure (cm H2O)');
    title(description);
    legend('Alarm','Pressure','Max Pressure','Min Pressure','RR','Max envelope','Min envelope');

    % Lock pan/scroll to x dimension only
    ax = gca; ax.Interactions = [panInteraction('Dimensions','x') zoomInteraction('Dimensions','x') dataTipInteraction];

end
