#!/bin/zsh

curl -v -i -H "content-type: text/xml" http://emuslice12:8080/cdw/factcollection -d "<soapenv:Envelope xmlns:soapenv='http://schemas.xmlsoap.org/soap/envelope/' xmlns:ws='http://ws.dw.csc.cms.cern.org/' xmlns:mod='http://www.cern.ch/cms/csc/dw/model' xmlns:ont='http://www.cern.ch/cms/csc/dw/ontology'>
   <soapenv:Header/>
   <soapenv:Body>
      <ws:input>
         <factCollection>
            <mod:source>LOCAL_DQM</mod:source>
            <!--mod:requestId>123</mod:requestId-->
            <mod:slidingTmbTriggerCounterFact>
               <mod:time>2010-01-01T01:01:01</mod:time>
               <mod:component_id>VMEm4_06</mod:component_id>
               <mod:severity>INFO</mod:severity>
               <mod:descr>Sliding TMB trigger counter</mod:descr>
               <mod:alctCount>123</mod:alctCount>
               <mod:clctCount>1234567890</mod:clctCount>
               <mod:lctCount>234567890</mod:lctCount>
               <mod:l1aCount>34567890</mod:l1aCount>
               <mod:windowWidth>5</mod:windowWidth>
            </mod:slidingTmbTriggerCounterFact>
            <mod:cumulativeTmbTriggerCounterFact>
               <mod:time>2010-01-01T01:01:02</mod:time>
               <mod:component_id>ME+1/1/01</mod:component_id>
               <mod:severity>INFO</mod:severity>
               <mod:descr>Cumulative TMB trigger counter</mod:descr>
               <mod:alctCount>0987654321</mod:alctCount>
               <mod:clctCount>987654321</mod:clctCount>
               <mod:lctCount>87654321</mod:lctCount>
               <mod:l1aCount>7654321</mod:l1aCount>
            </mod:cumulativeTmbTriggerCounterFact>
            <mod:cumulativeTmbTriggerCounterFact>
               <mod:time>2010-01-02T01:01:02</mod:time>
               <mod:component_id>ME+1/2/01</mod:component_id>
               <mod:severity>INFO</mod:severity>
               <mod:descr>Cumulative TMB trigger counter</mod:descr>
               <mod:alctCount>0987654321</mod:alctCount>
               <mod:clctCount>987654321</mod:clctCount>
               <mod:lctCount>87654321</mod:lctCount>
               <mod:l1aCount>7654321</mod:l1aCount>
            </mod:cumulativeTmbTriggerCounterFact>
         </factCollection>
      </ws:input>
   </soapenv:Body>
</soapenv:Envelope>"

echo ""
