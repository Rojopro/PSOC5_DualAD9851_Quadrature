# PSOC5_DualAD9851_Quadrature
This project aims to generate a Sine and Cosine with configurable frequency using two AD9851 chips

* Both AD9851 chips needs to be driven by the same clock, here generated by the PSOC. This implies unsoldering the Xtal if you bought an AD985X module.
* Do not daisy-chain the clock and FQ_UD signals, they need to have the same length (see application notes AN-587)

Special thanks : 
* http://ganymedeham.blogspot.com/2017/04/getting-it-staright-ad985x-arduino.html
* https://github.com/angadgill/dds_controller
