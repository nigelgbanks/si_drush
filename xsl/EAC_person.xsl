<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
<xsl:stylesheet version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="eac">
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
            <xsl:if test="eac:p_related_entity1/text()">
                <cpfRelation>
                    <relationEntry>
                        <xsl:value-of select="eac:p_related_entity1"/>
                    </relationEntry>
                    <descriptiveNote>
                        <p>
                            <xsl:value-of select="eac:p_related_entity1_description"/>
                        </p>
                    </descriptiveNote>
                </cpfRelation>
            </xsl:if>
            <xsl:if test="eac:p_related_entity2/text()">
                <cpfRelation>
                    <relationEntry>
                        <xsl:value-of select="eac:p_related_entity2"/>
                    </relationEntry>
                    <descriptiveNote>
                        <p>
                            <xsl:value-of select="eac:p_related_entity2_description"/>
                        </p>
                    </descriptiveNote>
                </cpfRelation>
            </xsl:if>
            <xsl:if test="eac:p_related_entity3/text()">
                <cpfRelation>
                    <relationEntry>
                        <xsl:value-of select="eac:p_related_entity3"/>
                    </relationEntry>
                    <descriptiveNote>
                        <p>
                            <xsl:value-of select="eac:p_related_entity3_description"/>
                        </p>
                    </descriptiveNote>
                </cpfRelation>
            </xsl:if>
            <xsl:if test="eac:p_related_material/text()">
                <resourceRelation>
                    <xsl:for-each select="eac:p_related_material/eac:DATA[normalize-space(text())]">
                        <relationEntry>
                            <xsl:value-of select="text()"/>
                        </relationEntry>
                    </xsl:for-each>
                </resourceRelation>
            </xsl:if>
        </relations>
    </xsl:template>
    <!--EAC Control section-->
    <xsl:template name="control">
        <control xmlns="urn:isbn:1-931666-33-4">
            <recordId>
                <xsl:value-of select="eac:personID"/>
            </recordId>
            <maintenanceStatus>
                <xsl:value-of select="eac:p_maintenanceStatus"/>
            </maintenanceStatus>
            <publicationStatus>
                <xsl:value-of select="eac:p_publicationStatus"/>
            </publicationStatus>
            <maintenanceAgency>
                <agencyName>
                    <xsl:value-of select="eac:p_maintenance_agency"/>
                </agencyName>
            </maintenanceAgency>
            <maintenanceHistory>
                <xsl:if test="eac:p_eventDateTime1/text()">
                    <maintenanceEvent>
                        <eventType>created</eventType>
                        <eventDateTime>
                            <xsl:value-of select="eac:p_eventDateTime1"/>
                        </eventDateTime>
                        <agentType>human</agentType>
                        <!-- assuming all agents are human as the data does not seem to specify-->
                        <agent>
                            <xsl:value-of select="eac:p_maintenance_history_agent"/>
                        </agent>
                        <eventDescription>Record Created</eventDescription>
                        <!-- eventDateTime1 corresponds to creation according to supplemental spreadsheet provided by Smithsonian-->
                    </maintenanceEvent>
                </xsl:if>
                <xsl:if test="eac:p_eventDateTimet2/text()">
                    <maintenanceEvent>
                        <eventType>revised</eventType>
                        <eventDateTime>
                            <xsl:value-of select="eac:p_eventDateTimet2"/>
                        </eventDateTime>
                        <agentType>human</agentType>
                        <!-- assuming all agents are human as the data does not seem to specify-->
                        <agent>
                            <xsl:value-of select="eac:p_maintenance_history_agent"/>
                        </agent>
                        <eventDescription>Record Edited</eventDescription>
                        <!-- eventDateTime2 corresponds to editing according to supplemental spreadsheet provided by Smithsonian-->
                    </maintenanceEvent>
                </xsl:if>
                <xsl:if test="eac:p_eventDateTime3/text()">
                    <maintenanceEvent>
                        <eventType>derived</eventType>
                        <eventDateTime>
                            <xsl:value-of select="eac:p_eventDateTime3"/>
                        </eventDateTime>
                        <agentType>human</agentType>
                        <!-- assuming all agents are human as the data does not seem to specify-->
                        <agent>
                            <xsl:value-of select="eac:p_maintenance_history_agent"/>
                        </agent>
                        <eventDescription>Record Harvested</eventDescription>
                        <!-- eventDateTime3 corresponds to Harvesting according to supplemental spreadsheet provided by Smithsonian-->
                    </maintenanceEvent>
                </xsl:if>
            </maintenanceHistory>
            <sources>
                <source>
                    <xsl:for-each select="eac:p_source[normalize-space(text())]">
                        <sourceEntry>
                            <xsl:value-of select="text()"/>
                        </sourceEntry>
                    </xsl:for-each>
                </source>
            </sources>
        </control>
    </xsl:template>
    <!-- EAC description section-->
    <xsl:template name="main">
        <cpfDescription xmlns="urn:isbn:1-931666-33-4">
            <identity>
                <entityId>
                    <xsl:value-of select="eac:personID"/>
                </entityId>
                <entityType>person</entityType>
                <nameEntry localType="primary">
                    <part localType="fullname">
                        <xsl:value-of select="eac:p_primary_name"/>
                    </part>
                </nameEntry>
                <nameEntry>
                    <part localType="forename">
                        <xsl:value-of select="eac:p_firstname"/>
                    </part>
                    <part localType="middle">
                        <xsl:value-of select="eac:p_middle_initial"/>
                    </part>
                    <part localType="surname">
                        <xsl:value-of select="eac:p_lastname"/>
                    </part>
                </nameEntry>
                <xsl:if test="eac:p_alt_name1/text()">
                    <nameEntry localType="alt">
                        <part>
                            <xsl:value-of select="eac:p_alt_name1"/>
                        </part>
                    </nameEntry>
                </xsl:if>
                <xsl:if test="eac:p_alt_name2/text()">
                    <nameEntry localType="alt">
                        <part>
                            <xsl:value-of select="eac:p_alt_name2"/>
                        </part>
                    </nameEntry>
                </xsl:if>
                <xsl:if test="eac:p_alt_name3/text()">
                    <nameEntry localType="alt">
                        <part>
                            <xsl:value-of select="eac:p_alt_name3"/>
                        </part>
                    </nameEntry>
                </xsl:if>
            </identity>
            <description>
                <existDates>
                    <dateRange>
                        <fromDate>
                            <xsl:value-of select="eac:p_exist_date_b"/>
                        </fromDate>
                        <!--based on the supplemental spreadsheet these are correct but looking at the data they seem backwards-->
                        <toDate>
                            <xsl:value-of select="eac:p_exist_date_d"/>
                        </toDate>
                    </dateRange>
                </existDates>
                <xsl:for-each select="eac:p_occupation/eac:DATA[normalize-space(text())]">
                    <occupation>
                        <term>
                            <xsl:value-of select="text()"/>
                        </term>
                    </occupation>
                </xsl:for-each>
                <place>
                    <address>
                        <xsl:if test="eac:p_address1">
                            <addressLine localType="street1"><xsl:value-of select="eac:p_address1"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_address2">
                           <addressLine localType="street2"><xsl:value-of select="eac:p_address2"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_city">
                            <addressLine localType="city"><xsl:value-of select="eac:p_city"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_state">
                            <addressLine localType="state"><xsl:value-of select="eac:p_state"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_postalcode">
                            <addressLine localType="postalcode"><xsl:value-of select="eac:p_postalcode"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_country">
                            <addressLine localType="country"><xsl:value-of select="eac:p_country"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_URL">
                            <addressLine localType="url"><xsl:value-of select="eac:p_URL"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_phone">
                            <addressLine localType="phone"><xsl:value-of select="eac:p_phone"/></addressLine>
                        </xsl:if>
                        <xsl:if test="eac:p_email_address">
                            <addressLine localType="email"><xsl:value-of select="eac:p_email_address"/></addressLine>
                        </xsl:if>
                    </address>
                </place>
                <languagesUsed>
                    <xsl:if test="normalize-space(eac:p_language1)">
                        <languageUsed>
                            <language languageCode="{eac:p_language1_code}">
                                <xsl:value-of select="eac:p_language1"/>
                            </language>
                            <script scriptCode="{eac:p_language1_script_code}"><xsl:value-of select="eac:p_language1_script"/></script>
                        </languageUsed>
                    </xsl:if>
                    <xsl:if test="normalize-space(eac:p_language2)">
                        <languageUsed>
                            <language languageCode="{eac:p_language2_code}">
                                <xsl:value-of select="eac:p_language2"/>
                            </language>
                            <script scriptCode="{eac:p_language2_script_code}"><xsl:value-of select="eac:p_language2_script"/></script>
                        </languageUsed>
                    </xsl:if>
                    <xsl:if test="normalize-space(eac:p_language3)">
                        <languageUsed>
                            <language languageCode="{eac:p_language3_code}">
                                <xsl:value-of select="eac:p_language3"/>
                            </language>
                            <script scriptCode="{eac:p_language3_script_code}"><xsl:value-of select="eac:p_language3_script"/></script>
                        </languageUsed>
                    </xsl:if>
                </languagesUsed>
                <biogHist>
                    <p>
                        <xsl:value-of select="normalize-space(eac:p_bio_history)"/>
                    </p>
                </biogHist>
            </description>
            <xsl:call-template name="relations"/>
        </cpfDescription>
    </xsl:template>
</xsl:stylesheet>
