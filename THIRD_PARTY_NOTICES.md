# Third-party notices

This repository is an **additive derivative** of OpenBCI firmware and vendors
upstream source. The notices below are reproduced as the MIT License requires.
The original license files are also **retained in full** inside the vendored
directory, and the root [`LICENSE`](LICENSE) carries the same attribution.

## OpenBCI Cyton Library (vendored)

The OpenBCI Cyton Library is vendored at `firmware/OpenBCI_Cyton_Library_custom/`
as an additive fork of v3.1.5. Its original MIT license is retained in full at
`firmware/OpenBCI_Cyton_Library_custom/LICENSE.md`. Our changes are additive and
the stock behavior is preserved.

Source: https://github.com/OpenBCI/OpenBCI_Cyton_Library

```
The MIT License (MIT)

Copyright (c) 2015 OpenBCI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## OpenBCI_GUI (reference, not redistributed)

The impedance ohms formula and the `z`-command sequence referenced in the docs
are from OpenBCI_GUI (MIT, Copyright (c) 2018 OpenBCI). No OpenBCI_GUI source is
copied into this repository.

Source: https://github.com/OpenBCI/OpenBCI_GUI

## Texas Instruments ADS1299 datasheet (reference, not redistributed)

The `SDATAC` / `WREG` / `RDATAC` register-command semantics and the lead-off
drive parameters are referenced from the Texas Instruments ADS1299 datasheet
(SBAS499C). Documentation reference only; no Texas Instruments material is
redistributed here.

Source: https://www.ti.com/lit/gpn/ads1299
