import os

if __name__ == "__main__":
    SPI_SPEED = 20 * 1000 * 1000
    I2C_SPEED = 400 * 1000

    print("SPI Speed: {} Hz".format(SPI_SPEED))
    print("I2C Speed: {} Hz".format(I2C_SPEED))

    spi_period = 1 / SPI_SPEED
    i2c_period = 1 / I2C_SPEED

    num_bits = 8
    num_bytes_to_transfer = 3 * 4 # Typical IMU data transfer

    total_time_spi = spi_period * num_bits * (1 + num_bytes_to_transfer)
    total_time_i2c = i2c_period * num_bits * 2 * num_bytes_to_transfer

    print("Total time for SPI: {} us".format(total_time_spi * 1000 * 1000))
    print("Total time for I2C: {} us".format(total_time_i2c * 1000 * 1000))
    print("SPI is {} times faster than I2C".format(total_time_i2c / total_time_spi))
