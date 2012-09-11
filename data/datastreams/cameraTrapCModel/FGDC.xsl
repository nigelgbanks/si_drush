<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:date="http://exslt.org/dates-and-times"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="eac"
    extension-element-prefixes="date">
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <xsl:param name="label">default</xsl:param>
    <!-- Entry Point -->
    <xsl:template match="/">
        <metadata xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:noNamespaceSchemaLocation="http://www.fgdc.gov/schemas/metadata/fgdc-std-001-1998.xsd">
            <idinfo>
                <citation>
                    <citeinfo>
                        <origin>default</origin>
                        <pubdate>
                            <xsl:value-of select="substring(date:date-time(), 1, 4)"/>
                        </pubdate>
                        <title>
                            <xsl:value-of select="$label"/>
                        </title>
                    </citeinfo>
                </citation>
                <descript>
                    <abstract>default</abstract>
                    <purpose>default</purpose>
                    <supplinf>default</supplinf>
                </descript>
                <timeperd>
                    <timeinfo>
                        <rngdates>
                            <begdate>0000</begdate>
                            <enddate>0000</enddate>
                        </rngdates>
                    </timeinfo>
                    <current>default</current>
                </timeperd>
                <status>
                    <progress>In work</progress>
                    <update>default</update>
                </status>
                <spdom>
                    <bounding>
                        <westbc>0.0</westbc>
                        <eastbc>0.0</eastbc>
                        <northbc>0.0</northbc>
                        <southbc>0.0</southbc>
                    </bounding>
                </spdom>
                <keywords>
                    <theme>
                        <themekt>unknown</themekt>
                        <themekey>unknown</themekey>
                    </theme>
                </keywords>
                <accconst>unknown</accconst>
                <useconst>unknown</useconst>
            </idinfo>
            <metainfo>
                <metd>0000</metd>
                <metc>
                    <cntinfo>
                        <cntorgp>
                            <cntorg>unknown</cntorg>
                        </cntorgp>
                        <cntaddr>
                            <addrtype>unknown</addrtype>
                            <city>unknown</city>
                            <state>unknown</state>
                            <postal>unknown</postal>
                        </cntaddr>
                        <cntvoice>unknown</cntvoice>
                    </cntinfo>
                </metc>
                <metstdn>unknown</metstdn>
                <metstdv>unknown</metstdv>
            </metainfo>
        </metadata>
    </xsl:template>
</xsl:stylesheet>
