import json
import numpy as np
import os

# Load the initial scene JSON
with open('initial_scene.json', 'r') as file:
    scene = json.load(file)

# Constants
total_frames = 240
fps = 24
rotation_angle = 360 / total_frames  # Total rotation divided by number of frames
sphere_start_frame = fps  # 1 second to start moving
float_duration = fps  # 1 second to float up
roll_duration = fps  # 1 second to roll down
sphere_ramp_start = [-0.4, 0.65, 1.3]  # Adjusted start position at ramp
sphere_ramp_float_end = [0.4, 0.45, 1.3]  # Position at top of ramp
sphere_ramp_roll_end = [0.0, -0.05, 1.0]  # End position at ramp

cylinder_start_frame = sphere_start_frame + float_duration + roll_duration
cylinder_animation_duration = fps  # 1 second for cylinder animation
cylinder_initial_position = [0.45, 0.5, 1.3]  # Initial position of the cylinder
cylinder_end_position = [0.2, -0.5, 0.8]  # End position of the cylinder (adjust as needed)
cylinder_rotation_axis = [1, 0, 1]  # Axis of rotation (adjust as needed)

# Constants for sphere's return motion
sphere_return_start_frame = cylinder_start_frame
sphere_return_duration = 2 * fps  # 2 seconds for sphere's return motion
sphere_return_end_position = [0.0, 0.45, 1.0]  # Middle of the table

sphere_index = 20
cylinder_index = 21

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

# Linear interpolation function
def lerp(start, end, t):
    return start + t * (end - start)

# Function for quadratic interpolation
def quadratic_interpolation(start, end, t):
    return start + (end - start) * (t ** 2)

def move_and_rotate_cylinder(cylinder, start_frame, current_frame, duration):
    if start_frame <= current_frame < start_frame + duration:
        t = (current_frame - start_frame) / duration
        # Quadratic translation
        new_pos = quadratic_interpolation(np.array(cylinder_initial_position), np.array(cylinder_end_position), t)
        cylinder['center'] = new_pos.tolist()
        # Rotation (adjust angle as needed)
        angle = 90 * t ** 2  # Quadratic rotation
        cylinder['axis'] = list(rotate_point(cylinder_rotation_axis, [0, 1, 0], angle))

def return_motion_sphere(sphere, start_frame, current_frame, duration):
    if start_frame <= current_frame < start_frame + duration:
        t = (current_frame - start_frame) / duration
        # Linear interpolation for return motion
        new_pos = lerp(np.array(sphere_ramp_roll_end), np.array(sphere_return_end_position), t)
        sphere['center'] = new_pos.tolist()

# Animation loop
for frame in range(total_frames):
    # Rotate camera
    camera_pos = scene['camera']['position']
    look_at = scene['camera']['lookAt']
    scene['camera']['position'] = rotate_point(camera_pos, look_at, (30 / 360) * np.sin(rotation_angle * 4 * np.pi / 360))

    # Swing light source above scene
    light = scene['scene']['lightsources'][0]
    light_pos = light['position']
    light_swing_point = light_pos + np.array([0, 0.3, 0])
    light['position'] = rotate_point(light_pos, light_swing_point, (30 / 360) * np.cos(rotation_angle * 6 * np.pi / 360))

    # Sphere animation
    sphere = scene['scene']['shapes'][sphere_index]  # Assuming sphere is the 20th shape
    if sphere['type'] != 'sphere':
        raise Exception('Shape is not a sphere.')
    if sphere_start_frame <= frame < sphere_start_frame + float_duration:
        t = (frame - sphere_start_frame) / float_duration
        sphere['center'] = list(quadratic_interpolation(np.array(sphere_ramp_start), np.array(sphere_ramp_float_end), t))
    elif sphere_start_frame + float_duration <= frame < sphere_start_frame + float_duration + roll_duration:
        t = (frame - (sphere_start_frame + float_duration)) / roll_duration
        sphere['center'] = list(quadratic_interpolation(np.array(sphere_ramp_float_end), np.array(sphere_ramp_roll_end), t))
    else:
        return_motion_sphere(sphere, sphere_return_start_frame, frame, sphere_return_duration)

    cylinder = scene['scene']['shapes'][cylinder_index]
    if cylinder['type'] != 'cylinder':
        raise Exception('Shape is not a cylinder.')
    move_and_rotate_cylinder(cylinder, cylinder_start_frame, frame, cylinder_animation_duration)

    # Write to file
    with open(f'animation2/frame_{frame:03}.json', 'w') as outfile:
        json.dump(scene, outfile, indent=4)

print("Animation frames generated.")
