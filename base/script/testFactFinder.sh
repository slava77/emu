sendSOAP -v  -u emuslice02 -p 20210 -A emu::daq::manager::Application -I 0 -b "
<esd:factRequestCollection xmlns:esd='http://www.cern.ch/cms/csc/dw/model'>
    <esd:requestId>98765</esd:requestId>
    <esd:factRequest>
        <esd:component_id>emu::daq::rui::Application00</esd:component_id>
        <esd:component_id>RUI00</esd:component_id>
        <esd:factType>ApplicationStatusFact</esd:factType>
    </esd:factRequest>
    <esd:factRequest>
        <esd:component_id>emu::daq::rui::Application01</esd:component_id>
        <esd:component_id>RUI01</esd:component_id>
        <esd:factType>ApplicationStatusFact</esd:factType>
    </esd:factRequest>
    <esd:factRequest>
        <esd:component_id>emu::daq::manager::Application</esd:component_id>
        <esd:component_id>DAQManager</esd:component_id>
        <esd:factType>LocalDAQStatusFact</esd:factType>
    </esd:factRequest>
</esd:factRequestCollection>"
