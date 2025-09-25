import numpy as np
import matplotlib.pyplot as plt
from scipy.ndimage import gaussian_filter

# Generate random particles for demonstration
num_particles = 500
particles = np.random.rand(num_particles, 2) * 100  # Random positions in 100x100 grid
masses = np.ones(num_particles)  # Assume unit mass for simplicity

# Define grid resolution
grid_size = 100
grid = np.zeros((grid_size, grid_size))

# Define smoothing length
h = 5.0

# Compute density on the grid
for x, y in particles:
    # Compute the grid cell indices
    i = int(x)
    j = int(y)
    # Add contribution to the grid
    if 0 <= i < grid_size and 0 <= j < grid_size:
        grid[i, j] += 1

# Apply Gaussian filter to smooth the density field
smoothed_grid = gaussian_filter(grid, sigma=h)

# Plot the result
plt.imshow(smoothed_grid, extent=(0, 100, 0, 100), origin='lower', cmap='jet')
plt.colorbar(label='Density')
plt.title('Smoothed Density Field')
plt.xlabel('x')
plt.ylabel('y')
plt.show()
