# Illinois RapidAlarm: Emergency ventilator sensor and alarm system

*Developed at the Grainger College of Engineering at the University of Illinois at Urbana-Champaign with support from Carle Health and Creative Thermal Solutions, Inc.*

![Illinois RapidAlarm](https://raw.githubusercontent.com/rapidalarm/rapidalarm.github.io/master/pictures/labeled_photos.png)

The Illinois RapidAlarm is a sensor and alarm module for use with emergency pressure-cycled ventilators such as the [Illinois RapidVent](https://rapidvent.grainger.illinois.edu/). More information and documentation is available at the [Illinois RapidAlarm project website](http://rapidalarm.github.io).

2020 Copyright University of Illinois Board of Trustees. Software is licensed under the MIT License; all other designs and documentation are licensed under the CC BY 4.0 license: https://creativecommons.org/licenses/by/4.0/

Please see the [legal disclaimer](https://rapidalarm.github.io/#!legal.md) on the project website.

### Repository Structure

- `code/` - firmware for monitor ATMega, simulations
  - `data/` - labeled alarm test data from artificial lung
  - `firmware/` - ATmega328 alarm firmware
  - `rapidalarm/` - Python plotting tools and test suite
- `cad/` - mechanical CAD files, enclosures, etc.
    - `enclosure_lasercut/` - enclosure design files for lasercutter
    - `enclosure_3dprint/` - enclosure design files for 3d printer
- `pcb/` - KiCAD design files
  - `pcb_proto/` - through hole board variant, based on ATmega328
  - `pcb_dip/` - surface mount board variant, based on ATmega32u4
