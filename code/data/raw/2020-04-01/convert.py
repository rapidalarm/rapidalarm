import pandas as pd
import numpy as np
from scipy.signal import resample

for path in (
        'exit_port_leak', 'first_test', 'lung_disconnect',
        'lung_partial_disconnect', 'source_disconnect'
    ):

    data = np.loadtxt(path, '\t')
    # convert ADC reading to cmh20
    data = 10.197 * (data / 1024 - 0.04) / 0.09

    df = pd.DataFrame(
        {
            'pressure': data[:, 1], # only second col contains valid information
            'flow': None,
            'true_pip': None,
            'true_peep': None,
            'true_rr': None
        }
    )

    with open('../../' + path + '.csv', 'w') as f:
        f.write('#samplerate,133\n')
        df.to_csv(f, index=False, header=True)
