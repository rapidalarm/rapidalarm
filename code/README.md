### Directory Structure

- `firmware/` - monitor firmware
- `rapidalarm/` - alarm algorithm test suite
- `data/` - test data

### Installation

Clone this repo.  From the `code/` directory:

Linux/OSX

    pip install .
    
Windows

1. [Install Visual C++ build tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
2. Restart
3. Run the above

### Usage

Run algorithm on recorded ventilator waveform and plot PIP/PEEP estimates

    rapidalarm plot data/ventmap_0282.csv
    
Alternatively you can import the library and do your own investigation

    >>> from rapidalarm.rapidalarm import run_pandas
    >>> df = run_pandas('data/ventmap_0282.csv')
