<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
    <xsl:stylesheet version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:eac="http://www.filemaker.com/fmpdsoresult"
        exclude-result-prefixes="eac">
   <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
       
        <xsl:template match="/">
                
            <xsl:for-each select="//eac:ROW">
                <eac-cpf xmlns="urn:isbn:1-931666-33-4"
                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                    xmlns:xlink="http://www.w3.org/1999/xlink"
                    xsi:schemaLocation="urn:isbn:1-931666-33-4 http://eac.staatsbibliothek-berlin.de/schema/cpf.xsd">
                <xsl:call-template name="control"/>               
                <xsl:call-template name="main"/>      
                </eac-cpf>                    
            </xsl:for-each>
               
        </xsl:template>
        <!-- EAC relationships-->
        <xsl:template name="relations">
            
                <relations xmlns="urn:isbn:1-931666-33-4">
                        <xsl:for-each select="e_participant_person">
                            <cpfRelation >
                        <relationEntry>
                            <xsl:value-of select="."/>
                        </relationEntry>   
                                </cpfRelation>
                        </xsl:for-each>                            
                    <xsl:for-each select="eac:e_participant_organization">
                        <cpfRelation >
                            <relationEntry>
                                <xsl:value-of select="."/>
                            </relationEntry>   
                        </cpfRelation>
                    </xsl:for-each>                    
                            <xsl:for-each select="eac:e_place/eac:DATA">   
                                <xsl:if test="./text()">
                                    <cpfRelation >
                            <xsl:variable name="PLACE" select="."/>       <!--not sure what to do with records that have multiple }} -->                    
                            <xsl:variable name="VOC" select="substring-after($PLACE,' }} ')"/>    
                            <xsl:variable name="TERM" select="substring-before($PLACE, ' }} ')"/>                              
                            <placeEntry vocabularySource="{$VOC}"><xsl:value-of select="$TERM"/></placeEntry>
                                    </cpfRelation>
                                </xsl:if>
                            </xsl:for-each>                                                   
                </relations>                    
                        
        </xsl:template>
        <!--EAC Control section-->
        <xsl:template name="control">
            <control xmlns="urn:isbn:1-931666-33-4">
                <recordId><xsl:value-of select="eac:expeditionID"/></recordId>
                <!--<publicationStatus><xsl:value-of select="eac:e_publicationStatus"/></publicationStatus> this seems to break validation as I think it requires and otherid which i don't think is in the data supplied-->
                <maintenanceStatus><xsl:value-of select="eac:e_maintenanceStatus"/></maintenanceStatus>
                <maintenanceAgency>
                  
                    <agencyName><xsl:value-of select="eac:e_maintenance_agency"/></agencyName>
                </maintenanceAgency>
                <maintenanceHistory>
                    <xsl:if test="eac:e_eventDateTime1/text()">
                        <maintenanceEvent>
                            <eventType>created</eventType>
                            <eventDateTime><xsl:value-of select="eac:e_eventDateTime1"/></eventDateTime>
                            <agentType>human</agentType> <!-- assuming all agents are human as the data does not seem to specify-->
                            <agent><xsl:value-of select="eac:e_maintenance_history_agent"/></agent>                            
                            <eventDescription>Record Created</eventDescription> <!-- eventDateTime1 corresponds to creation according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                    <xsl:if test="eac:e_eventDateTime2/text()">
                        <maintenanceEvent>
                            <eventType>revised</eventType>
                            <eventDateTime><xsl:value-of select="eac:e_eventDateTime2"/></eventDateTime>
                            <agentType>human</agentType>
                            <agent><xsl:value-of select="eac:e_maintenance_history_agent"/></agent>   
                            <eventDescription>Record Edited</eventDescription><!-- eventDateTime2 corresponds to editing according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                    <xsl:if test="eac:e_eventDateTime3/text()">
                        <maintenanceEvent>
                            <eventType>harvested</eventType>
                            <eventDateTime><xsl:value-of select="eac:e_eventDateTime3"/></eventDateTime>
                            <agentType>human</agentType>
                            <agent><xsl:value-of select="eac:e_maintenance_history_agent"/></agent>   
                            <eventDescription>Record Harvested</eventDescription><!-- eventDateTime3 corresponds to Harvesting according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                </maintenanceHistory>
            </control>
            
        </xsl:template>
       <!-- EAC description section-->
        <xsl:template name="main">
            <cpfDescription xmlns="urn:isbn:1-931666-33-4">
                <identity>
                    <entityType>corporateBody</entityType>
                    <nameEntry localType="primary">
                        <part><xsl:value-of select="eac:e_primary_name"/></part>                      
                    </nameEntry>
                    <xsl:if test="eac:org_abbreviation/text()">
                        <nameEntry localType="abbreviation">
                        <part><xsl:value-of select="eac:org_abbreviation"/></part>                      
                    </nameEntry>
                        </xsl:if>
                    <xsl:if test="eac:e_alt_name1/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:e_alt_name1"/></part>
                        </nameEntry>
                    </xsl:if>
                    <xsl:if test="eac:e_alt_name2/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:e_alt_name2"/></part>
                        </nameEntry>
                    </xsl:if>
                    <xsl:if test="eac:e_alt_name3/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:e_alt_name3"/></part>
                        </nameEntry>
                    </xsl:if>                  
                </identity>
                <description>
                    <xsl:if test="eac:e_exist_date1/text()">
                    <existDates> 
                        <dateRange> 
                            <fromDate ><xsl:value-of select="eac:e_exist_date_start"/></fromDate>
                            <toDate ><xsl:value-of select="eac:e_exist_date_end"/></toDate> 
                        </dateRange> 
                        <xsl:if test="eac:e_exist_date_description/text()">
                            <descriptiveNote><xsl:value-of select="eac:e_exist_date_description"/></descriptiveNote>
                        </xsl:if>
                    </existDates> 
                        </xsl:if>
                    <xsl:for-each select="eac:e_subject/eac:DATA">   
                        <xsl:if test="./text()">
                        <xsl:variable name="SUBJECT" select="."/>                               
                        <xsl:variable name="VOC" select="substring-after($SUBJECT,' }} ')"/>    
                        <xsl:variable name="TERM" select="substring-before($SUBJECT, ' }} ')"/>   
                                                
                        <function localType="{$VOC}"><term><xsl:value-of select="$TERM"/></term></function>
                            </xsl:if>
                    </xsl:for-each>          
                    <biogHist>
                        <xsl:if test="eac:e_note/text()">
                        <abstract><xsl:value-of select="eac:e_note"/></abstract>
                            </xsl:if>
                        <xsl:if test="eac:e_description/text()">
                        <p><xsl:value-of select="eac:e_description"/></p>
                            </xsl:if>
                    </biogHist>
                </description>
                <xsl:call-template name="relations"/>
                </cpfDescription>              
        </xsl:template>
       
</xsl:stylesheet>