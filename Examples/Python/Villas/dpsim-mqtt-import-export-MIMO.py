# This example attempts to read a sine signal generated by VILLASnode via a MQTT connection as well as a shared memory connection.
# dpsim then exports several values over these connections which should be read by VILLASnode and written to a file.

# Due to problems with the shared memory node in VILLAS, this example currently does not run correctly!

import sys
import os.path
import logging
import json
import time

from datetime import datetime
from villas.node.node import Node as VILLASnode

import dpsimpy
import dpsimpyvillas

base = os.path.splitext(os.path.basename(sys.argv[0]))[0]
log = logging.getLogger(base)

def villas():

    log_filename=datetime.now().strftime(f'{base}-villas-%y-%m-%d_%H_%M_%S.log')

    nodes = {
        'mqtt': {
            'type': 'mqtt',
            'format': 'json',
            'host': 'mqtt',
            'in': {
                'subscribe': '/dpsim-mqtt',
                'signals':  [
                    {
                        'name': 'v1',
                        'type': 'complex'
                    },
                    {
                        'name': 'v2_mag',
                        'type': 'float'
                    }
                ]
            },
            'out': {
                'publish': '/mqtt-dpsim'
            }
        },
        'shmem': {
            'type': 'shmem',
            'in': {
                'name': '/dpsim-shmem',
                'signals':  [
                    {
                        'name': 'v1',
                        'type': 'complex'
                    },
                    {
                        'name': 'v2_mag',
                        'type': 'float'
                    }
                ]
            },
            'out': {
                'name': '/shmem-dpsim'
            }
        },
        'file1': {
            'type': 'file',
            'uri': f'{base}-results-%y-%m-%d_%H_%M_%S.csv'
        },
        'file2': {
            'type': 'file',
            'uri': f'{base}-results-%y-%m-%d_%H_%M_%S_2.csv'
        },
        'sine' : {
            'type': 'signal',
            'signal': 'sine',
            'rate': 10,
            'frequency': 0.1,
            'amplitude': 50000,  
            'offset': 100000
        },
    }

    paths = [
        {
            'in': 'mqtt',
            'out': 'file1',
            'hooks': [{'type':'print'}]
        },
        {
            'in': 'shmem',
            'out': 'file2',
            'hooks': [{'type':'print'}]
        },
        {
            'in': 'sine',
            'out': ['mqtt', 'shmem'],
            'hooks': [{'type':'print'}]
        },
    ]

    config = {
        'nodes': nodes,
        'paths': paths
    }
    config['nodes']['mqtt']['out']['hooks'] = [{'type':'print'}]

    log.info('VILLASnode config: \n%s', json.dumps(config, indent=2))

    return VILLASnode(config=config, log_filename=log_filename)

def dpsim():
    # Parameters
    V_nom = 20e3
    p_load_nom = 100e3
    q_load_nom = 50e3
    line_resistance = 0.05
    line_inductance = 0.1
    line_capacitance = 0.1e-6
    name = 'dpsim_mqtt_import_export'

    # Nodes and Components
    n1 = dpsimpy.sp.SimNode('n1', dpsimpy.PhaseType.Single)
    n2 = dpsimpy.sp.SimNode('n2', dpsimpy.PhaseType.Single)

    extnet = dpsimpy.sp.ph1.NetworkInjection('Slack')
    extnet.set_parameters(voltage_set_point=V_nom)
    extnet.set_base_voltage(V_nom)
    extnet.modify_power_flow_bus_type(dpsimpy.PowerflowBusType.VD)

    line = dpsimpy.sp.ph1.PiLine('PiLine')
    line.set_parameters(R=line_resistance, L=line_inductance, C=line_capacitance)
    line.set_base_voltage(V_nom)

    load = dpsimpy.sp.ph1.Load('Load')
    load.set_parameters(active_power=p_load_nom, reactive_power=q_load_nom, nominal_voltage=V_nom)
    load.modify_power_flow_bus_type(dpsimpy.PowerflowBusType.PQ)

    extnet.connect([n1])
    line.connect([n1, n2])
    load.connect([n2])
    system = dpsimpy.SystemTopology(50, [n1, n2], [extnet, line, load])

    sim = dpsimpy.RealTimeSimulation(name, dpsimpy.LogLevel.debug)
    sim.set_system(system)
    sim.set_domain(dpsimpy.Domain.SP)
    sim.set_solver(dpsimpy.Solver.NRP)
    sim.set_time_step(0.1)
    sim.set_final_time(10)
    sim.do_init_from_nodes_and_terminals(False)
    
    logger = dpsimpy.Logger(name)
    sim.add_logger(logger)
    sim.log_attr('n1', 'v')
    sim.log_attr('n2', 'v')

    intf_config = {
        "type": "mqtt",
        "format": "json",
        "host": "mqtt",
        "in": {
            "subscribe": "/mqtt-dpsim"
        },
        "out": {
            "publish": "/dpsim-mqtt"
        }
    }

    intf_config_2 = {
        "type": "shmem",
        "in": {
            "name": "/shmem-dpsim"
        },
        "out": {
            "name": "/dpsim-shmem"
        }
    }

    intf = dpsimpyvillas.InterfaceVillas(name="dpsim-mqtt", config=intf_config)
    sim.add_interface(intf)

    intf2 = dpsimpyvillas.InterfaceVillas(name="dpsim-shmem", config=intf_config_2)
    sim.add_interface(intf2)
    sim.import_attr('Load', 'P', 0, intf=intf)
    sim.export_attr('n1', 'v', 0, intf=intf)
    sim.export_attr('n2', 'v', 1, dpsimpy.AttrModifier.mag, intf=intf)
    sim.import_attr('Load', 'Q', 0, intf=intf2)
    sim.export_attr('n1', 'v', 0, intf=intf2)
    sim.export_attr('n2', 'v', 1, dpsimpy.AttrModifier.mag, intf=intf2)
  
    return sim, intf, intf2

def test_shmem_import_export():
    logging.basicConfig(format='[%(asctime)s %(name)s %(levelname)s] %(message)s', datefmt='%H:%M:%S', level=logging.INFO)

    sim, intf, intf2 = dpsim() #intf needs to be extracted from the dpsim-function since the interface object gets deleted otherwise leading to SegFault when starting the simulation
    node = villas()

    node.start()
    sim.run(1)
    node.stop()

if __name__ == '__main__':
    test_shmem_import_export()