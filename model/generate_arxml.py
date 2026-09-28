import xml.etree.ElementTree as ET
from xml.dom import minidom

def generate_arxml():
    # 1. Root AUTOSAR Element with AUTOSAR 4.2.2 standard schema
    root = ET.Element("AUTOSAR", {
        "xmlns": "http://autosar.org/schema/r4.0",
        "xmlns:xsi": "http://www.w3.org/2001/XMLSchema-instance",
        "xsi:schemaLocation": "http://autosar.org/schema/r4.0 AUTOSAR_4-2-2.xsd"
    })

    ar_packages = ET.SubElement(root, "AR-PACKAGES")

    # -------------------------------------------------------------
    # 2. DataTypes Package (uint16)
    # -------------------------------------------------------------
    pkg_datatypes = ET.SubElement(ar_packages, "AR-PACKAGE")
    ET.SubElement(pkg_datatypes, "SHORT-NAME").text = "DataTypes"
    dt_elements = ET.SubElement(pkg_datatypes, "ELEMENTS")

    impl_type = ET.SubElement(dt_elements, "IMPLEMENTATION-DATA-TYPE")
    ET.SubElement(impl_type, "SHORT-NAME").text = "uint16"
    ET.SubElement(impl_type, "CATEGORY").text = "VALUE"

    # -------------------------------------------------------------
    # 3. PortInterfaces Package (Sender-Receiver Interface)
    # -------------------------------------------------------------
    pkg_interfaces = ET.SubElement(ar_packages, "AR-PACKAGE")
    ET.SubElement(pkg_interfaces, "SHORT-NAME").text = "PortInterfaces"
    if_elements = ET.SubElement(pkg_interfaces, "ELEMENTS")

    sr_if = ET.SubElement(if_elements, "SENDER-RECEIVER-INTERFACE")
    ET.SubElement(sr_if, "SHORT-NAME").text = "VehicleSpeed_I"
    data_elements = ET.SubElement(sr_if, "DATA-ELEMENTS")

    var_dataprototype = ET.SubElement(data_elements, "VARIABLE-DATA-PROTOTYPE")
    ET.SubElement(var_dataprototype, "SHORT-NAME").text = "VehicleSpeed"
    type_ref = ET.SubElement(var_dataprototype, "TYPE-TREF", {"DEST": "IMPLEMENTATION-DATA-TYPE"})
    type_ref.text = "/DataTypes/uint16"

    # -------------------------------------------------------------
    # 4. ComponentTypes Package (SWC, Port, Behavior, Runnable)
    # -------------------------------------------------------------
    pkg_components = ET.SubElement(ar_packages, "AR-PACKAGE")
    ET.SubElement(pkg_components, "SHORT-NAME").text = "ComponentTypes"
    comp_elements = ET.SubElement(pkg_components, "ELEMENTS")

    swc = ET.SubElement(comp_elements, "APPLICATION-SW-COMPONENT-TYPE")
    ET.SubElement(swc, "SHORT-NAME").text = "SpeedSensor_SWC"

    # Provided Port (P-Port)
    ports = ET.SubElement(swc, "PORTS")
    p_port = ET.SubElement(ports, "P-PORT-PROTOTYPE")
    ET.SubElement(p_port, "SHORT-NAME").text = "PP_VehicleSpeed"
    if_ref = ET.SubElement(p_port, "PROVIDED-INTERFACE-TREF", {"DEST": "SENDER-RECEIVER-INTERFACE"})
    if_ref.text = "/PortInterfaces/VehicleSpeed_I"

    # Internal Behavior
    behavior = ET.SubElement(swc, "INTERNAL-BEHAVIORS")
    ib = ET.SubElement(behavior, "SWC-INTERNAL-BEHAVIOR")
    ET.SubElement(ib, "SHORT-NAME").text = "SpeedSensor_InternalBehavior"

    # Events container
    events = ET.SubElement(ib, "EVENTS")
    timing_event = ET.SubElement(events, "TIMING-EVENT")
    ET.SubElement(timing_event, "SHORT-NAME").text = "TE_20ms"
    ET.SubElement(timing_event, "PERIOD").text = "0.02"
    start_runnable_ref = ET.SubElement(timing_event, "START-ON-EVENT-REF", {"DEST": "RUNNABLE-ENTITY"})
    start_runnable_ref.text = "/ComponentTypes/SpeedSensor_SWC/SpeedSensor_InternalBehavior/SpeedSensor_ReadSpeed"

    # Runnables container
    runnables = ET.SubElement(ib, "RUNNABLES")
    runnable = ET.SubElement(runnables, "RUNNABLE-ENTITY")
    ET.SubElement(runnable, "SHORT-NAME").text = "SpeedSensor_ReadSpeed"
    ET.SubElement(runnable, "CAN-BE-INVOKED-CONCURRENTLY").text = "false"
    ET.SubElement(runnable, "SYMBOL").text = "SpeedSensor_ReadSpeed"

    # Data Access for the Runnable (Write access to PP_VehicleSpeed)
    data_write_points = ET.SubElement(runnable, "DATA-WRITE-ACCESSS")
    var_access = ET.SubElement(data_write_points, "VARIABLE-ACCESS")
    ET.SubElement(var_access, "SHORT-NAME").text = "DWA_VehicleSpeed"
    accessed_var = ET.SubElement(var_access, "ACCESSED-VARIABLE")
    autosar_var = ET.SubElement(accessed_var, "AUTOSAR-VARIABLE-IREF")
    p_port_ref = ET.SubElement(autosar_var, "PORT-PROTOTYPE-REF", {"DEST": "P-PORT-PROTOTYPE"})
    p_port_ref.text = "/ComponentTypes/SpeedSensor_SWC/PP_VehicleSpeed"
    target_data_ref = ET.SubElement(autosar_var, "TARGET-DATA-PROTOTYPE-REF", {"DEST": "VARIABLE-DATA-PROTOTYPE"})
    target_data_ref.text = "/PortInterfaces/VehicleSpeed_I/VehicleSpeed"

    # -------------------------------------------------------------
    # 5. Format and Save to File
    # -------------------------------------------------------------
    raw_xml = ET.tostring(root, encoding="utf-8")
    parsed = minidom.parseString(raw_xml)
    pretty_xml = parsed.toprettyxml(indent="  ", encoding="utf-8")

    output_filename = "SpeedSensor_Model.arxml"
    with open(output_filename, "wb") as f:
        f.write(pretty_xml)

    print(f"\n[SUCCESS] ARXML generated successfully: {output_filename}")

if __name__ == "__main__":
    generate_arxml()