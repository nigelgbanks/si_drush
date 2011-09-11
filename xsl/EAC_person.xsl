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
            <xsl:if test="eac:p_related_entity1/text()">
            <relations xmlns="urn:isbn:1-931666-33-4">
                <cpfRelation>
                    <relationEntry>
                        <xsl:value-of select="eac:p_related_entity1"/>
                    </relationEntry>
                    <descriptiveNote>
                        <p><xsl:value-of select="eac:p_related_entity1_description"/></p>
                    </descriptiveNote>
                </cpfRelation>            
                <xsl:if test="eac:p_related_entity2/text()">
                    <cpfRelation>
                        <relationEntry>
                            <xsl:value-of select="eac:p_related_entity2"/>
                        </relationEntry>
                        <descriptiveNote>
                            <p><xsl:value-of select="eac:p_related_entity2_description"/></p>
                        </descriptiveNote>
                    </cpfRelation>
                </xsl:if>
                <xsl:if test="eac:p_related_entity3/text()">
                    <cpfRelation>
                        <relationEntry>
                            <xsl:value-of select="eac:p_related_entity3"/>
                        </relationEntry>
                        <descriptiveNote>
                            <p><xsl:value-of select="eac:p_related_entity3_description"/></p>
                        </descriptiveNote>
                    </cpfRelation>
                </xsl:if>
                </relations>
            </xsl:if>
            
        </xsl:template>
        <!--EAC Control section-->
        <xsl:template name="control">
            <control xmlns="urn:isbn:1-931666-33-4">
                <recordId><xsl:value-of select="eac:personID"/></recordId>
                <!--<publicationStatus><xsl:value-of select="eac:p_publicationStatus"/></publicationStatus> this seems to break validation as I think it requires and otherid which i don't think is in the data supplied-->
                <maintenanceStatus><xsl:value-of select="eac:p_maintenanceStatus"/></maintenanceStatus>
                <maintenanceAgency>
                    <!--<agencyCode>AU-BS</agencyCode>-->
                    <agencyName><xsl:value-of select="eac:p_maintenance_agency"/></agencyName>
                </maintenanceAgency>
                <maintenanceHistory>
                    <xsl:if test="eac:p_eventDateTime1/text()">
                        <maintenanceEvent>
                            <eventType>created</eventType>
                            <eventDateTime><xsl:value-of select="eac:p_eventDateTime1"/></eventDateTime>
                            <agentType>human</agentType> <!-- assuming all agents are human as the data does not seem to specify-->
                            <agent><xsl:value-of select="eac:p_maintenance_history_agent"/></agent>                            
                            <eventDescription>Record Created</eventDescription> <!-- eventDateTime1 corresponds to creation according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                    <xsl:if test="eac:p_eventDateTime2/text()">
                        <maintenanceEvent>
                            <eventType>revised</eventType>
                            <eventDateTime><xsl:value-of select="eac:p_eventDateTime2"/></eventDateTime>
                            <agentType>human</agentType> <!-- assuming all agents are human as the data does not seem to specify-->
                            <agent><xsl:value-of select="eac:p_maintenance_history_agent"/></agent>        
                            <eventDescription>Record Edited</eventDescription><!-- eventDateTime2 corresponds to editing according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                    <xsl:if test="eac:p_eventDateTime3/text()">
                        <maintenanceEvent>
                            <eventType>harvested</eventType>
                            <eventDateTime><xsl:value-of select="eac:p_eventDateTime3"/></eventDateTime>
                            <agentType>human</agentType> <!-- assuming all agents are human as the data does not seem to specify-->
                            <agent><xsl:value-of select="eac:p_maintenance_history_agent"/></agent>        
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
                    <entityType>person</entityType>
                    <nameEntry localType="primary">
                        <part localType="fullname"><xsl:value-of select="eac:p_primary_name"/></part>
                        <!--<authorizedForm>AARC2</authorizedForm>-->
                    </nameEntry>
                    <nameEntry>
                        <part localType="surname"><xsl:value-of select="eac:p_firstname"/></part>
                        <part localType="forename"><xsl:value-of select="eac:p_lastname"/></part>
                        <part localType="middle"><xsl:value-of select="eac:p_middle_initial"/></part>
                        <!--<authorizedForm>AARC2</authorizedForm>-->
                    </nameEntry>
                    <xsl:if test="eac:p_alt_name1/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:p_alt_name1"/></part>
                        </nameEntry>
                    </xsl:if>
                    <xsl:if test="eac:p_alt_name2/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:p_alt_name2"/></part>
                        </nameEntry>
                    </xsl:if>
                    <xsl:if test="eac:p_alt_name3/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:p_alt_name3"/></part>
                        </nameEntry>
                    </xsl:if>
                  
                </identity>
                <description>
                    <existDates> 
                        <dateRange> 
                            <fromDate ><xsl:value-of select="eac:p_exist_date_b"/></fromDate> <!--based on the supplemental spreadsheet these are correct but looking at the data they seem backwards-->
                            <toDate ><xsl:value-of select="eac:p_exist_date_d"/></toDate> 
                        </dateRange> 
                    </existDates> 
                    <occupation>
                        <xsl:for-each select="eac:p_occupation">
                            <term><xsl:value-of select="eac:p_occupation"/></term>
                        </xsl:for-each>
                    </occupation>
                    <biogHist>
                        <p><xsl:value-of select="eac:p_bio_history"/></p>
                    </biogHist>
                </description>
                <xsl:call-template name="relations"/>
                </cpfDescription>    
            
        </xsl:template>
       
</xsl:stylesheet>