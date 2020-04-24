# Rapidalarm - Low cost monitor for emergency COVID-19 ventilators

- [Project Website](http://rapidalarm.github.io)

### Repository Structure

- `code/` - firmware for monitor ATMega, simulations
  - `data/` - labeled alarm test data from artificial lung
  - `firmware/` - ATmega328 alarm firmware
  - `python/` - alarm algorithm test suite
- `cad/` - mechanical CAD files, enclosures, etc.
    - `enclosure_lasercut/` - enclosure design files for lasercutter
    - `enclosure_3dprint/` - enclosure design files for 3d printer
- `pcb/` - KiCAD design files
  - `pcb_proto/` - through hole board variant, based on ATmega328
  - `pcb_dip/` - surface mount board variant, based on ATmega32u4
