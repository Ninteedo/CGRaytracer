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
ball_ramp_duration = fps  # 1 second to roll down ramp
ball_table_roll_duration = fps * 3  # 3 seconds to roll on table
sphere_start_location = [-0.3, 0.45, 0.8]  # Initial position of the sphere
sphere_ramp_start = [-0.4, 0.65, 1.3]  # Position at top of ramp
sphere_ramp_bottom = [0.4, 0.45, 1.3]  # Position at bottom of ramp
sphere_table_roll_end = [-0.3, 0.45, 0.8]  # Sphere end roll position

cylinder_start_frame = sphere_start_frame + float_duration + ball_ramp_duration
cylinder_animation_duration = fps  # 1 second for cylinder animation
cylinder_wall_bash_duration = fps * 0.5
cylinder_initial_position = [0.45, 0.5, 1.3]  # Initial position of the cylinder
cylinder_initial_axis = np.array([0, 1, 0])  # Initial axis of rotation
cylinder_wall_bash_position = [0.95, 0.4, 1.1]  # Position when cylinder bumps wall
cylinder_wall_bash_axis = np.array([-0.8, 0.6, 0])  # Axis of rotation when cylinder bumps wall
cylinder_end_position = [0.8, -0.5, 0.9]  # End position of the cylinder (adjust as needed)
cylinder_end_axis = np.array([0.8, 0, 0.6])  # End axis of rotation (adjust as needed)

# Constants for sphere's return motion
sphere_return_start_frame = cylinder_start_frame
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

def move_and_rotate_cylinder(cylinder, current_frame):
    if current_frame < cylinder_start_frame:
        cylinder['center'] = cylinder_initial_position
    elif cylinder_start_frame <= current_frame < cylinder_start_frame + cylinder_wall_bash_duration:
        t = (current_frame - cylinder_start_frame) / cylinder_wall_bash_duration
        # Quadratic translation
        new_pos = quadratic_interpolation(np.array(cylinder_initial_position), np.array(cylinder_wall_bash_position), t)
        cylinder['center'] = new_pos.tolist()
        # Rotation (adjust angle as needed)
        new_axis = lerp(np.array(cylinder_initial_axis), np.array(cylinder_wall_bash_axis), t)
        new_axis = new_axis / np.linalg.norm(new_axis) # Normalize axis
        cylinder['axis'] = new_axis.tolist()
    elif current_frame < cylinder_start_frame + cylinder_wall_bash_duration + cylinder_animation_duration:
        t = (current_frame - cylinder_start_frame - cylinder_wall_bash_duration) / cylinder_animation_duration
        # Quadratic translation
        new_pos = quadratic_interpolation(np.array(cylinder_wall_bash_position), np.array(cylinder_end_position), t)
        cylinder['center'] = new_pos.tolist()
        # Rotation (adjust angle as needed)
        new_axis = lerp(np.array(cylinder_wall_bash_axis), np.array(cylinder_end_axis), t).tolist()
        new_axis = new_axis / np.linalg.norm(new_axis) # Normalize axis
        cylinder['axis'] = new_axis.tolist()

def move_sphere(sphere, current_frame):
    if  current_frame < sphere_start_frame:
        sphere['center'] = sphere_start_location
    elif current_frame < sphere_start_frame + float_duration:
        t = (current_frame - sphere_start_frame) / float_duration
        start = np.array(sphere_start_location)
        end = np.array(sphere_ramp_start)
        x = start[0] + (end[0] - start[0]) * np.sin(t * np.pi / 2)
        y = start[1] + (end[1] - start[1]) * np.sin(t * np.pi / 2)
        z = start[2] + (end[2] - start[2]) * t ** 2
        sphere['center'] = list(quadratic_interpolation(np.array(sphere_start_location), np.array(sphere_ramp_start), t))
    elif current_frame < sphere_start_frame + float_duration + ball_ramp_duration:
        t = (current_frame - (sphere_start_frame + float_duration)) / ball_ramp_duration
        sphere['center'] = list(quadratic_interpolation(np.array(sphere_ramp_start), np.array(sphere_ramp_bottom), t))
    elif current_frame < sphere_start_frame + float_duration + ball_ramp_duration + ball_table_roll_duration:
        t = (current_frame - (sphere_start_frame + float_duration + ball_ramp_duration)) / ball_table_roll_duration
        start = np.array(sphere_ramp_bottom)
        end = np.array(sphere_table_roll_end)
        x = start[0] + (end[0] - start[0]) * np.sin(t * np.pi / 2) ** 2
        y = lerp(start[1], end[1], t)
        z = start[2] + (end[2] - start[2]) * np.sin(t * np.pi / 2) ** (1/2)
        sphere['center'] = [x, y, z]
    else:
        sphere['center'] = sphere_table_roll_end

    # print(sphere['center'])


# Animation loop
for frame in range(total_frames):
    # Rotate camera
    camera_pos = scene['camera']['position']
    look_at = scene['camera']['lookAt']
    scene['camera']['position'] = rotate_point(camera_pos, look_at, np.cos(np.radians(frame) * 1.5))

    # Swing light source above scene
    light = scene['scene']['lightsources'][0]
    light_pos = light['position']
    light['position'][0] = light_pos[0] + (-0.02 * np.cos(np.radians(frame) * 3))

    # Sphere animation
    sphere = scene['scene']['shapes'][sphere_index]  # Assuming sphere is the 20th shape
    if sphere['type'] != 'sphere':
        raise Exception('Shape is not a sphere.')
    move_sphere(sphere, frame)

    cylinder = scene['scene']['shapes'][cylinder_index]
    if cylinder['type'] != 'cylinder':
        raise Exception('Shape is not a cylinder.')
    move_and_rotate_cylinder(cylinder, frame)

    # Write to file
    with open(f'animation2/frame_{frame:03}.json', 'w') as outfile:
        json.dump(scene, outfile, indent=4)

print("Animation frames generated.")
