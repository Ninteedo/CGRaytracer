import json
import numpy as np
import os

# Load the initial scene JSON
with open('initial_scene.json', 'r') as file:
    scene = json.load(file)

# Constants
total_frames = 240
rotation_angle = 360 / total_frames  # Total rotation divided by number of frames

# Function to rotate a point around another point
def rotate_point(point, center, angle):
    # Convert angle to radians
    rad = np.radians(angle)
    # Translate point to origin
    temp_point = [point[0] - center[0], point[1] - center[1], point[2] - center[2]]
    # Perform rotation
    rotated_point = [
        temp_point[0] * np.cos(rad) - temp_point[2] * np.sin(rad),
        temp_point[1],
        temp_point[0] * np.sin(rad) + temp_point[2] * np.cos(rad)
    ]
    # Translate back
    return [rotated_point[0] + center[0], rotated_point[1] + center[1], rotated_point[2] + center[2]]

# Animation loop
for frame in range(total_frames):
    # Rotate camera
    camera_pos = scene['camera']['position']
    look_at = scene['camera']['lookAt']
    scene['camera']['position'] = rotate_point(camera_pos, look_at, rotation_angle)

    # Rotate light source in opposite direction
    light_pos = scene['scene']['lightsources'][0]['position']
    scene['scene']['lightsources'][0]['position'] = rotate_point(light_pos, look_at, -rotation_angle)

    # Animate textured sphere
    # Example: move along x-axis
    scene['scene']['shapes'][4]['center'][0] += 0.01  # Adjust this based on desired motion

    # Change color of large sphere
    large_sphere_color = scene['scene']['shapes'][0]['material']['diffusecolor']
    scene['scene']['shapes'][0]['material']['diffusecolor'] = [color + 0.001 for color in large_sphere_color]

    # Change roughness of glass cylinder
    # Example: sine wave variation
    scene['scene']['shapes'][1]['material']['roughness'] = 0.5 * np.sin(2 * np.pi * frame / total_frames)

    # Write to file
    with open(f'animation/frame_{frame:03}.json', 'w') as outfile:
        json.dump(scene, outfile, indent=4)

print("Animation frames generated.")
