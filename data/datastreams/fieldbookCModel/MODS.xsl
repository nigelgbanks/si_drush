<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns="urn:isbn:1-931666-33-4"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:date="http://exslt.org/dates-and-times"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="eac"
    extension-element-prefixes="date">
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <xsl:param name="label">default</xsl:param>
    <xsl:param name="entity_id">default</xsl:param>
    <!-- Entry Point -->
    <xsl:template match="/">
        <eac-cpf xmlns="urn:isbn:1-931666-33-4" xmlns:xlink="http://www.w3.org/1999/xlink"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="urn:isbn:1-931666-33-4 http://eac.staatsbibliothek-berlin.de/schema/cpf.xsd">
            <control>
                <recordId>default</recordId>
                <maintenanceStatus>new</maintenanceStatus>
                <publicationStatus>inProcess</publicationStatus>
                <maintenanceAgency>
                    <agencyName>Smithsonian Institution Archives</agencyName>
                </maintenanceAgency>
                <maintenanceHistory>
                    <maintenanceEvent>
                        <eventType>created</eventType>
                        <eventDateTime><xsl:value-of select="substring-before(date:date-time(), 'T')"/></eventDateTime>
                        <agentType>human</agentType>
                        <agent>Smithsonian Institution Archives</agent>
                        <eventDescription>Record Created</eventDescription>
                    </maintenanceEvent>
                </maintenanceHistory>
            </control>
            <cpfDescription>
                <identity>
                    <entityId><xsl:value-of select="$entity_id"/></entityId>
                    <entityType>person</entityType>
                    <nameEntry localType="primary">
                        <part><xsl:value-of select="$label"/></part>
                    </nameEntry>
                    <nameEntry localType="full">
                        <part localType="forename"/>
                        <part localType="middle"/>
                        <part localType="surname"/>
                    </nameEntry>
                </identity>
                <description>
                    <existDates>
                        <dateRange>
                            <fromDate/>
                            <toDate/>
                        </dateRange>
                    </existDates>
                    <biogHist>
                        <p></p>
                    </biogHist>
                </description>
                <relations/>
            </cpfDescription>
        </eac-cpf>
    </xsl:template>
</xsl:stylesheet>
