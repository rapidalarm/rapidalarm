### Transducers

| Part       | Digikey Quantity | Mouser Quantity |
| ---------- | -------          | ------          |
| MPXV5100GC | 756              | 1029            |
| MPXV5100GP | 292              |                 |
| MPXV5100DP | 4500             | 2881            |
| MPXV5050GC |                  | 2820            |
| MPXV5050DP | 379              | 404             |
| MPXV7025   | 279              | 8264            |
| MPXV5010GC | 420              |                 |
| MPVZ5010GW | 425              |                 |
| MPXV4006DP | 276              |                 |
| MPXV4006GC | 245              | 200             |
| MPXV5004   | 1320             |                 |

- sdp800-125
- https://octopart.com/2smpp-02-omron-18985974?r=sp#Specs
- https://octopart.com/tbpdans015pgucv-honeywell-34872416?r=sp
- https://octopart.com/mpvz4006gw7u-nxp+semiconductors-70303583?r=sp
- differential sensors ~.01psi/1cmh20 - https://www.digikey.com/products/en/sensors-transducers/pressure-sensors-transducers/512?k=pressure+sensor&k=&pkeyword=pressure+sensor&sv=0&pv144=8614&pv144=6360&pv144=4027&pv144=2964&pv144=620&pv144=640&pv144=427967&pv144=650&pv144=662&pv144=666&pv144=683&pv144=427968&pv144=694&pv144=425047&pv144=719&pv144=725&pv144=728&pv144=729&pv144=427966&pv144=427965&pv144=786&pv144=6350&pv144=6876&pv144=427804&pv144=8073&pv144=8612&pv144=8613&pv144=8289&pv144=9508&pv276=174378&pv276=228720&pv276=206234&pv276=208636&pv276=208699&pv276=209065&pv276=209787&pv276=209799&pv276=209898&pv276=209941&pv276=209952&pv276=210520&pv276=211233&pv276=249171&pv276=249294&pv276=249306&pv276=249369&pv276=249379&sf=1&FV=-8%7C512&quantity=&ColumnSort=0&page=1&stock=1&pageSize=500
- [venturi tube calculator](https://www.pipeflowcalculations.com/venturi/calculator.xhtml)

### M. Stevens Questions

- considerations for pick-and-place
- local pick-and-place machines

- tvs diodes on buttons
- high esr bulk cap on supply
- wall wart has built-in mov
- panelizing design
- 062
- vscore 1 mil


#### Flow transducers
- https://octopart.com/d6f-ph0505ad3-omron-29408017?r=sp

### Things to Order
- https://www.digikey.com/product-detail/en/te-connectivity-alcoswitch-switches/2-1825910-7/450-1642-ND/1632528
- https://octopart.com/2smpp-02-omron-18985974?r=sp#Specs
- https://octopart.com/tbpdans015pgucv-honeywell-34872416?r=sp
- https://octopart.com/mpvz4006gw7u-nxp+semiconductors-70303583?r=sp

### Ordered
- https://www.digikey.com/short/zpq87p

- voltage reference 1.234V, 5x - https://www.digikey.com/product-detail/en/texas-instruments/LM385BLP-1-2/296-9558-5-ND/379850
- voltage reference 3V, 5x - https://www.digikey.com/product-detail/en/texas-instruments/LM4040D30ILPR/296-21583-1-ND/1494038
- resistor array, 10x - https://www.digikey.com/product-detail/en/bourns-inc/4608X-102-471LF/4608X-2-471LF-ND/1089144
- resistor array, 10x - https://www.digikey.com/product-detail/en/bourns-inc/4606X-102-681LF/4606X-2-681LF-ND/1089048
- 4 digit display, 2x - https://www.digikey.com/product-detail/en/lite-on-inc/LTC-4627JF/160-1548-5-ND/408221
- inductor, 10x - https://www.digikey.com/product-detail/en/bourns-inc/78F100J-TR-RC/78F100J-TR-RCCT-ND/6679613
- barrel supply, 2x - https://www.digikey.com/product-detail/en/phihong-usa/PSM03A-050-R/993-1236-ND/4031882
- barrel jack, 3x - https://www.digikey.com/product-detail/en/w%C3%BCrth-elektronik/694106301002/732-5930-ND/5047522
- micro usb, 3x - https://www.digikey.com/product-detail/en/w%C3%BCrth-elektronik/629105150521/732-5960-1-ND/5047555
- buttons, 20x - https://www.digikey.com/product-detail/en/c-k/PTS636-SK43-LFS/CKN12303-ND/10071716
- pressure transducer, 1x - https://www.digikey.com/product-detail/en/nxp-usa-inc/MPXV5050GC6T1/568-13603-2-ND/2186550
- pressure transducer, 1x - https://www.digikey.com/products/en?keywords=mpxv5100dp
- pressure transucer, 1x - https://www.digikey.com/product-detail/en/nxp-usa-inc/MPXV7002DP/MPXV7002DP-ND/1168436
  
### Design questions

- shield vs dedicated board - shield about same cost with added benefit of extensibility -> slightly larger, more complex
  - produce dedicated in quantity, design shield produce small amount
- smd vs thru hole
  - a
- reprogrammability - maybe, pogo pin port on back of board
  - dont care
- sourcing transducers
- serial printing/debugging - no, requires switch from atmega or serial-usb interface chip
- power connector - micro usb?  convenient for low quant, more difficult to solder than barrel

- alarm - SFM-27 https://www.youtube.com/watch?v=LLGMlErc0yQ
- esp8266 bulk US - https://www.ebay.com/itm/5X-ESP8266-ESP-12E-802-11n-WIFI-USB-Development-Board-Arduino-MicroPython-5-Pack/132532596048?hash=item1edb8eed50:g:Z3QAAOSwsnVdyOf-

- esp8266 bulk 
  - https://www.aliexpress.com/item/32665100123.html?spm=a2g0o.productlist.0.0.3ffc15495JaD23&algo_pvid=1e29147f-2a71-40df-94b8-4b58d9622e4f&algo_expid=1e29147f-2a71-40df-94b8-4b58d9622e4f-0&btsid=0ab6d69f15850880382543898e3676&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
  
- esp02 bulk
  - https://www.mouser.com/ProductDetail/Espressif-Systems/ESP-WROOM-02?qs=sGAEpiMZZMsRr7brxAGoXSSUPDSAjAiVc55QyFq0cPAl59eVNM2ypQ%3D%3D
  - https://www.digikey.com/product-detail/en/espressif-systems/ESP-WROOM-02/1904-1009-1-ND/8544304
  
- esp32 bulk
  - https://www.mouser.com/ProductDetail/Espressif-Systems/ESP32-WROOM-32-16MB?qs=sGAEpiMZZMsRr7brxAGoXSSUPDSAjAiVWYDCG5gOE4CudLdUnuGgeQ%3D%3D
  - https://www.mouser.com/ProductDetail/Espressif-Systems/ESP32-SOLO-1?qs=sGAEpiMZZMsRr7brxAGoXSSUPDSAjAiVLQnuuMBfLkMt7n12ghVrZw%3D%3D
  - https://www.digikey.com/product-detail/en/espressif-systems/ESP32-WROVER/1904-1007-1-ND/8544302

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

### Old Purchases
- displays
    - 2x - https://www.digikey.com/product-detail/en/lite-on-inc/LTC-4627JF/160-1548-5-ND/408221
- transducers
    - 2x - https://www.digikey.com/product-detail/en/nxp-usa-inc/MPXV5010GC7U/MPXV5010GC7U-ND/412949
    - https://www.digikey.com/product-detail/en/nxp-usa-inc/MPXV7007DP/MPXV7007DP-ND/1168438
    - https://www.digikey.com/product-detail/en/nxp-usa-inc/MPXV5010DP/MPXV5010DP-ND/531280
- buzzers
    - https://www.digikey.com/product-detail/en/pui-audio-inc/AI-3035-TWT-3V-R/668-1204-ND/1745457
    - 2x - https://www.digikey.com/product-detail/en/murata-electronics/PKM13EPYH4002-B0/490-4697-ND/1219328
- microcontrollers
    - https://www.digikey.com/product-detail/en/espressif-systems/ESP32-WROOM-32/1904-1010-1-ND/8544305
    - 2x - https://www.digikey.com/product-detail/en/microchip-technology/ATMEGA328P-PU/ATMEGA328P-PU-ND/1914589
    - https://www.digikey.com/product-detail/en/microchip-technology/ATMEGA328P-AUR/ATMEGA328P-AURCT-ND/3789455
- tactile switch
    - 6x - https://www.digikey.com/product-detail/en/omron-electronics-inc-emc-div/B3FS-1000P/SW423CT-ND/277814
    
- old stuff, dont order
    - https://www.digikey.com/product-detail/en/varitronix/VI-201-DP-RC-S/153-1003-ND/2375
    - https://www.digikey.com/product-detail/en/matrix-orbital/MOP-AL162A-BBTW-25J-3IN/635-1206-ND/9602839
    - https://www.digikey.com/product-detail/en/adafruit-industries-llc/1001/1528-1513-ND/5774239
    - https://www.digikey.com/product-detail/en/newhaven-display-intl/NHD-C0216CZ-FSW-FBW-3V3/NHD-C0216CZ-FSW-FBW-3V3-ND/2059234

### Power Supply
- https://www.digikey.com/products/en/power-supplies-external-internal-off-board/ac-dc-desktop-wall-adapters/130?k=power+supply&k=&pkeyword=power+supply&sv=0&sf=0&FV=-8%7C130&quantity=&ColumnSort=1000011&page=1&stock=1&pageSize=25

### Display Modules
- 3.3V, Serial, $8 - https://www.digikey.com/product-detail/en/newhaven-display-intl/NHD-C0216CZ-FSW-FBW-3V3/NHD-C0216CZ-FSW-FBW-3V3-ND/2059234
-


### Differential pressure sensors

##### Single-Ended
| Part        | Price | Quantity | Voltage | Range | Notes      |
| MPXV4006GC6 |       |          | 5V      | 60cm  | Integrated |
| MP3V5004GP  |       | 5000     | 3V      | 40cm  | Integrated |
| MPX5010GP   |       |          |         |       |            |

##### Differential

| Part        | Price | Quantity | Voltage | Range | Notes         |
| MP3V5004DP  |       |          | 3V      | 40cm  | Integrated    |
| MPX10DP     |       |          | 3V      | 100cm | Uncompensated |
| MP3V5010DP  |       |          | 3V      | 100cm | Integrated    |
| MPXV5010DP  |       |          | 5V      | 100cm | Integrated    |
| MP3V5004GC6 |       |          |         | 40    |               |
| MP3V5010GC6 |       |          | 3V      | 100cm | Integrated    |
