#!/bin/zsh
curl -v -i -H "content-type: text/xml" http://emuslice12:8080/FactCollectionInputService/FactCollectionInput -d "<soapenv:Envelope xmlns:soapenv='http://schemas.xmlsoap.org/soap/envelope/' xmlns:ws='http://ws.cdw.csc.cms.cern.ch/' xmlns:data='http://www.cern.org/cms/csc/dw/data'>
  <soapenv:Header/>
  <soapenv:Body>
     <ws:getFactCollection>
        <!--Optional:-->
        <factCollection>
           <data:source>Xmas</data:source>
           <!--You have a CHOICE of the next 2 items at this level-->
           <data:slidingTmbTriggerCounterFact>
              <data:time>2009-12-04T14:51:29.100102Z</data:time>
              <data:component>ME+1/1/1</data:component>
              <!--Optional:-->
              <data:severity>INFO</data:severity>
              <!--Optional:-->
              <data:descr>well, ALCT doesn't look very good</data:descr>
              <data:alctCount>0</data:alctCount>
              <data:clctCount>100</data:clctCount>
              <data:lctCount>50</data:lctCount>
              <data:l1aCount>30</data:l1aCount>
              <data:windowWidth>30</data:windowWidth>
           </data:slidingTmbTriggerCounterFact>
           <data:cumulativeTmbTriggerCounterFact>
              <data:time>2009-12-04T14:51:29.100102Z</data:time>
              <data:component>ME+1/1/2</data:component>
              <!--Optional:-->
              <data:severity>INFO</data:severity>
              <!--Optional:-->
              <data:descr>Looks nice</data:descr>
              <data:alctCount>10</data:alctCount>
              <data:clctCount>10</data:clctCount>
              <data:lctCount>5</data:lctCount>
              <data:l1aCount>3</data:l1aCount>
           </data:cumulativeTmbTriggerCounterFact>
        </factCollection>
     </ws:getFactCollection>
  </soapenv:Body>
</soapenv:Envelope>"

echo ""
