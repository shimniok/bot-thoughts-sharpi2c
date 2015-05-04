# Oversampling #

You can change the effective resolution of the conversion result which in turn increases the oversampling performed by the I2C Adapter.

[Register](Registers.md) 3, RESOLUTION, is user configurable and can be set to 10-16 inclusive. The I2C Adapter uses a 10-bit ADC. Increasing the effective resolution by n bits increases the oversampling by 4n.

You can read back the oversampling value from [register](Registers.md) 2.

# Filtering #

The firmware on the I2C adapter implements an exponential decay filter otherwise known as a [Leaky Integrator](http://en.wikipedia.org/wiki/Leaky_integrator).

The user configurable [register](Registers.md) 4, FILTER, determines how quickly or slowly the leaky integrator leaks.

A higher value performs more filtering, reducing bandwidth and noise. A lower value performs less filtering increasing response (bandwidth) and noise.