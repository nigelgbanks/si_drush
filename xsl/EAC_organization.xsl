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
            <xsl:if test="eac:o_parent_org/text()">
                <relations xmlns="urn:isbn:1-931666-33-4">
                    <cpfRelation cpfRelationType="hierarchical-parent">
                        <relationEntry>
                            <xsl:value-of select="eac:o_parent_org"/>
                        </relationEntry>                        
                    </cpfRelation>  
                </relations>                    
            </xsl:if>            
        </xsl:template>
        <!--EAC Control section-->
        <xsl:template name="control">
            <control xmlns="urn:isbn:1-931666-33-4">
                <recordId><xsl:value-of select="eac:organizationID"/></recordId>
                <!--<publicationStatus><xsl:value-of select="eac:o_publicationStatus"/></publicationStatus> this seems to break validation as I think it requires and otherid which i don't think is in the data supplied-->
                <maintenanceStatus><xsl:value-of select="eac:o_maintenanceStatus"/></maintenanceStatus>
                <maintenanceAgency>
                  
                    <agencyName><xsl:value-of select="eac:o_maintenance_agency"/></agencyName>
                </maintenanceAgency>
                <maintenanceHistory>
                    <xsl:if test="eac:o_eventDateTime1/text()">
                        <maintenanceEvent>
                            <eventType>created</eventType>
                            <eventDateTime><xsl:value-of select="eac:o_eventDateTime1"/></eventDateTime>
                            <agentType>human</agentType> <!-- assuming all agents are human as the data does not seem to specify-->
                            <agent><xsl:value-of select="eac:o_maintenance_history_agent"/></agent>                            
                            <eventDescription>Record Created</eventDescription> <!-- eventDateTime1 corresponds to creation according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                    <xsl:if test="eac:o_eventDateTime2/text()">
                        <maintenanceEvent>
                            <eventType>revised</eventType>
                            <eventDateTime><xsl:value-of select="eac:o_eventDateTime2"/></eventDateTime>
                            <agentType>human</agentType>
                            <agent><xsl:value-of select="eac:o_maintenance_history_agent"/></agent>   
                            <eventDescription>Record Edited</eventDescription><!-- eventDateTime2 corresponds to editing according to supplemental spreadsheet provided by Smithsonian-->
                        </maintenanceEvent>
                    </xsl:if>
                    <xsl:if test="eac:o_eventDateTime3/text()">
                        <maintenanceEvent>
                            <eventType>harvested</eventType>
                            <eventDateTime><xsl:value-of select="eac:o_eventDateTime3"/></eventDateTime>
                            <agentType>human</agentType>
                            <agent><xsl:value-of select="eac:o_maintenance_history_agent"/></agent>   
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
                        <part><xsl:value-of select="eac:o_primary_name"/></part>                      
                    </nameEntry>
                    <xsl:if test="eac:org_abbreviation/text()">
                        <nameEntry localType="abbreviation">
                        <part><xsl:value-of select="eac:org_abbreviation"/></part>                      
                    </nameEntry>
                        </xsl:if>
                    <xsl:if test="eac:o_alt_name1/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:o_alt_name1"/></part>
                        </nameEntry>
                    </xsl:if>
                    <xsl:if test="eac:o_alt_name2/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:o_alt_name2"/></part>
                        </nameEntry>
                    </xsl:if>
                    <xsl:if test="eac:o_alt_name3/text()">
                        <nameEntry localType="alt">
                            <part><xsl:value-of select="eac:o_alt_name3"/></part>
                        </nameEntry>
                    </xsl:if>                  
                </identity>
                <description>
                    <xsl:if test="eac:o_exist_date1/text()">
                    <existDates> 
                        <dateRange> 
                            <fromDate ><xsl:value-of select="eac:o_exist_date1"/></fromDate> <!--based on the supplemental spreadsheet these are correct but looking at the data they seem backwards-->
                            <toDate ><xsl:value-of select="eac:o_exist_date2"/></toDate> 
                        </dateRange> 
                    </existDates> 
                        </xsl:if>
                    <places><place><address>
                        <xsl:if test="eac:o_address1/text()">
                      <addressLine localType="line1">
                        <xsl:value-of select = "eac:o_address1"/>
                      </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_address2/text()">
                    <addressLine localType="line2">
                         <xsl:value-of select = "eac:o_address2"/>
                    </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_city/text()">
                            <addressLine localType="city">
                         <xsl:value-of select = "eac:o_city"/>
                    </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_state/text()">
                            <addressLine localType="state">
                         <xsl:value-of select = "eac:o_state"/>
                    </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_postalcode/text()">
                            <addressLine localType="postalcode">
                         <xsl:value-of select = "eac:o_postalcode"/>
                    </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_country/text()">
                            <addressLine localType="country">
                         <xsl:value-of select = "eac:o_country"/>
                    </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_phone/text()">
                            <addressLine localType="phone">
                         <xsl:value-of select = "eac:o_phone"/>
                    </addressLine>
                        </xsl:if>
                        <xsl:if test="eac:o_email_address/text()">
                            <addressLine localType="email">
                         <xsl:value-of select = "eac:o_email_address"/>
                    </addressLine>
                        </xsl:if>
                         <xsl:if test="eac:o_URL/text()">
                            <addressLine localType="url">
                         <xsl:value-of select = "eac:o_URL"/>
                    </addressLine>
                        </xsl:if>
                    </address></place></places>
                    <biogHist>
                        <xsl:if test="eac:o_note/text()">
                        <abstract><xsl:value-of select="eac:o_note"/></abstract>
                            </xsl:if>
                        <xsl:if test="eac:o_description/text()">
                        <p><xsl:value-of select="eac:o_description"/></p>
                            </xsl:if>
                    </biogHist>
                </description>
                <xsl:call-template name="relations"/>
                </cpfDescription>              
        </xsl:template>
       
</xsl:stylesheet>