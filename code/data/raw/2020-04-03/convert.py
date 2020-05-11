import pandas as pd
import numpy as np

for path in ('noncompliant', 'low_flow', 'low_rate'):

    data = np.loadtxt(path, '\t')
    df = pd.DataFrame(
        {
            'pressure': data[:, 1], # only second col contains valid information
            'flow': None,
            'pip': None,
            'peep': None,
            'rr': None
        }
    )

    with open('../../' + path + '.csv', 'w') as f:
        f.write('#samplerate,100\n')
        df.to_csv(f, index=False, header=True)

