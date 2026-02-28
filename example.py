import raytracing
import pandas as pd

data = pd.read_csv('combined_data.csv')

sensor_names = [
    'jv-edmk1-1A0189-flarm',
    'dr-DR05602446A6001222W-RemoteID-Wi-Fi-wifi-2.4-ghz',
    'dr-DR05602446A6001222W-RemoteID-Wi-Fi-wifi-5-ghz',
    'jv-edmk2-1A0189-flarm',
    'dr-DR05602446A6001220W-RemoteID-Wi-Fi-wifi-2.4-ghz',
    'dr-DR05602446A6001220W-RemoteID-Wi-Fi-wifi-5-ghz',
    'dr-DR05602446A6001222W-RemoteID-NAN-wifi-2.4-ghz',
    'dr-DR05602446A6001222W-RemoteID-NAN-wifi-5-ghz',
    'dr-DR05602446A6001222-DJI-Matrice-200-V2-OcuSync-radio-2.4-ghz',
    'dr-DR05602446A6001222-DJI-Matrice-200-V2-OcuSync-radio-5-ghz'
]


# Einlesen der Positionsdaten
data = pd.read_csv('combined_data.csv')

for sensor in sensor_names:
    # Extraktion der relevanten Positionsdaten
    relevant_data = data[
        [
        # Sensorstandort
        sensor + '_loc_sensor_position_x',
        sensor + '_loc_sensor_position_y',
        sensor + '_sensor_altitude_asl',

        # Drohnenposition
        'log_position_x',
        'log_position_y',
        'log_alti'
        ]
    ]
    # Entfernung aller Datensätze ohne Positionsdaten
    # beim Sensorstandort oder der Drohne
    relevant_data = relevant_data.dropna()


    start_utm_x = relevant_data[sensor + '_loc_sensor_position_x'].values[0]
    start_utm_y = relevant_data[sensor + '_loc_sensor_position_y'].values[0]
    start_utm_z = relevant_data[sensor + '_sensor_altitude_asl'].values[0]

    start_point = (start_utm_x, start_utm_y, start_utm_z)

    end_points = []

    for i, row in relevant_data.iterrows():
        end_point = (row[3], row[4], row[5])
        end_points.append( end_point )

    raytracing.raytracing_direct( start_point, end_points )
    #raytracing.raytracing_with_reflection( start_point, end_points, 'max_area', grid_resolution=0.2, max_threads=1 )
