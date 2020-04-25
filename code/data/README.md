# rapidalarm test data

### File structure

- `raw/` - raw data recordings

### CSV Format

The first two lines of the CSV file contain information about sample rate and column names

e.g.

    #samplerate,50
    pressure,flow,pip,peep,rr
    10.57,3.44,20.57,10.879999999999999,14.084507042253522
    10.55,5.36,20.57,10.879999999999999,14.084507042253522
    10.51,9.18,20.57,10.879999999999999,14.084507042253522
    ...

### Ventmap Data

A lot of ventilator waveform data can be found in the [ventMAP project repo](https://github.com/hahnicity/ventMAP/tree/master/ventmap/tests/samples), albeit a bit disorganized.  

These recordings were made using the PB-840 ventilator at 50Hz.  You can use the `ventmap2csv.py` tool to convert these `.csv_test` files into the csv format used by the rapidalarm Python library.

    python ventmap2csv.py input.csv_test output.csv
