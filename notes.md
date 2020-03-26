- alarm - SFM-27 https://www.youtube.com/watch?v=LLGMlErc0yQ
- esp8266 bulk US - https://www.ebay.com/itm/5X-ESP8266-ESP-12E-802-11n-WIFI-USB-Development-Board-Arduino-MicroPython-5-Pack/132532596048?hash=item1edb8eed50:g:Z3QAAOSwsnVdyOf-

- esp8266 bulk - https://www.aliexpress.com/item/32665100123.html?spm=a2g0o.productlist.0.0.3ffc15495JaD23&algo_pvid=1e29147f-2a71-40df-94b8-4b58d9622e4f&algo_expid=1e29147f-2a71-40df-94b8-4b58d9622e4f-0&btsid=0ab6d69f15850880382543898e3676&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_

- 0.2 Hz

- kivy signed build - https://groups.google.com/forum/#!topic/kivy-users/pBtbg8aSlNo
- kivy listview update - https://groups.google.com/forum/#!topic/kivy-users/D7kaXll-rj8

huge dirs:

- ~/.local/share/python-for-android
- ~/.buildozer

### when to raise alarm

- respiratory rate too high or too low

### Definitions

- [ventilator control modes](https://www.youtube.com/watch?v=gk_Qf-JAL84)
- [respiratory cycle example](https://www.youtube.com/watch?v=K0maLgTzIto)
- [pressure control ventilation](https://www.youtube.com/watch?v=h-I6XE6EfMY)

- WOB - work of breathing - 
- compliance - ΔV/ΔP - analogous to stiffness constant of spring (inverted)
- restrictive lung disease - condition in which compliance of lung is reduced.  Greater pressure needed to deliver same volume of air
- FIO2 - fractional inspiration of oxygen - percent of air volume oxygen
- CMV - continuous mandatory control
- IMV - intermittent mandatory control
- TCT - total cycle time (60 / rate)
- Ti - inspiration time
- Te - expiration time
- PC - pressure control
- AC - assist control
- PEEP - positive-end expiratory pressure


- ventilator modes
  - AC - assist control - ventilator control mode where specific volume is delivered to patient in each cycle
    - patient triggers ventilator
    - set tidal volume, instantaneous pressure dependent on lung compliance
    - e.g. 500-600cc of air delivered (8mL/kg bodyweight)
  - PC - pressure control - 
    - patient or ventilator trigger inspiration
    - set tidal pressure, instantaneous volume dependent on lung compliance
    - if compliance of lungs changes, inspired volume can change
  - CPAP - non-invasive ventilation, facemask instead of tracheal tube, sleep apnea
    - devices often have a PEEP setting
    - PEEP - positive-end expiratory pressure - positive pressure setting after expiration
      - prevents sleep apnea, tongue back
      - keeps alveoli open
      - set pressure (cm water pressure, e.g. 5-20 cm)
  - PS - pressure support
    - patient initiates all breaths (e.g. 5-15 cm pressure)
    
- 

### Example Orders for Ventilator
- AC 16 - assist control, backup rate 16 breaths per minute
  - rate: 16
  - tidal volume: 550mL
  - FIO2: 50%
  - PEEP: 5
  - optional: inspiratory flow
  
- PC
  - pressure control level (defined absolutely or amount above PEEP)
  - rate
  - Ti (amount of time pressure held high?)
  - PEEP
  - FiO2
  - Sensitivity
  

### Problems in AC Mode Ventilation

- [detecting respiratory cycle problems](https://www.youtube.com/watch?v=6Bdv7QhNNy4)

- assist control
  - high peak pressure (poor airway flow) (also called Pplat, MAP mean error press., Paw peak airway)
    - bronchiospasms
    - secretions
    - mucus plug
    - tracheal tube occlusion
  - high plateau pressure (low lung compliance)
    - pneumothorax
    - pulmonary edema
    - ARDS
    - pneumonia
