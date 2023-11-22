
<emos:sch cfv:name="C:/Users/haojie.fang/Downloads/Test Project Base_20221222/Test Project Base/Case_01/3_architecture_design/Case_01.cfv" project:name="C:/Users/haojie.fang/Downloads/Test Project Base_20221222/Test Project Base/Case_01/schedule_configuration/sample.sch" version="1">
    <task detatime="5" name="Calculator_NotifyCalEvent" oid="emos.sdk.calculator" periodTime="500" priority="1" runtime="20" timeout="-1" triggerTask="" type="0"/>
    <task detatime="5" name="Calculator_NotifyCalEvent2" oid="emos.sdk.calculator" periodTime="500" priority="1" runtime="20" timeout="-1" triggerTask="" type="0"/>
    <task detatime="5" name="Calculator_NotifyCalPin" oid="emos.sdk.calculator" periodTime="500" priority="1" runtime="20" timeout="-1" triggerTask="" type="0"/>
    <task detatime="5" name="Calculator_NotifyCalPin2" oid="emos.sdk.calculator" periodTime="500" priority="1" runtime="20" timeout="-1" triggerTask="" type="0"/>
    <task detatime="5" name="Client1_Event" oid="emos.sdk.client1" periodTime="500" priority="1" runtime="20" timeout="-1" triggerTask="Calculator_NotifyCalEvent" type="1"/>
    <task detatime="5" name="Client1_TestCallInprocI" oid="emos.sdk.client1" periodTime="1000" priority="1" runtime="50" timeout="-1" triggerTask="" type="0"/>
    <task detatime="5" name="Client1_TestCallInprocII" oid="emos.sdk.client1" periodTime="1000" priority="1" runtime="50" timeout="-1" triggerTask="" type="0"/>
    <task detatime="5" name="Client1_TestCallInprocIII" oid="emos.sdk.client1" periodTime="1000" priority="1" runtime="50" timeout="-1" triggerTask="" type="0"/>
    <roulette core="0" customPeriodTime="-1" detatime="-1" index="0" name="chainTask0" priority="1" timeoffset="20" timeout="-1" type="1">
        <task name="Calculator_NotifyCalEvent" starttime="0"/>
    </roulette>
    <roulette core="0" customPeriodTime="-1" detatime="-1" index="1" name="chainTask1" priority="1" timeoffset="20" timeout="-1" type="1">
        <task name="Calculator_NotifyCalEvent2" starttime="8"/>
    </roulette>
    <roulette core="0" customPeriodTime="-1" detatime="-1" index="2" name="chainTask2" priority="1" timeoffset="20" timeout="-1" type="1">
        <task name="Calculator_NotifyCalPin" starttime="0"/>
        <task name="Client1_Event" starttime="22"/>
    </roulette>
    <template core="0" detatime="-1" index="0" name="package0" priority="1" timeoffset="20" timeout="-1" type="1">
        <task name="Calculator_NotifyCalPin2" starttime="0"/>
        <task name="Calculator_NotifyCalPin" starttime="22"/>
    </template>
</emos:sch>
