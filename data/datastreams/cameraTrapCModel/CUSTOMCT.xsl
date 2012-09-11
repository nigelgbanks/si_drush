<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:date="http://exslt.org/dates-and-times"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="eac"
    extension-element-prefixes="date">
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <xsl:param name="label">default</xsl:param>
    <!-- Entry Point -->
    <xsl:template match="/">
        <cameraDeployment>
            <cameraSiteName>unknown</cameraSiteName>
            <cameraLongitudeNorth>unknown</cameraLongitudeNorth>
            <cameraLongitudeSouth>unknown</cameraLongitudeSouth>
            <cameraLatitudeEast>unknown</cameraLatitudeEast>
            <cameraLatitudeWest>unknown</cameraLatitudeWest>
            <elevation>none</elevation>
            <habitatType>none</habitatType>
            <cameraDeploymentBeginDate>unknown</cameraDeploymentBeginDate>
            <cameraDeploymentEndDate>unknown</cameraDeploymentEndDate>
            <cameraId>unknown</cameraId>
            <imageResolutionSetting>none</imageResolutionSetting>
            <cameraFailureDetails>none</cameraFailureDetails>
            <cameraCard>unknown</cameraCard>
            <detectionDistance>none</detectionDistance>
            <sensitivity>unknown</sensitivity>
            <quietPeriodSetting>unknown</quietPeriodSetting>
            <cameraHeight>none</cameraHeight>
            <cameraGroup>none</cameraGroup>
            <featureType>none</featureType>
            <baitType>unknown</baitType>
            <baitSchedule>unknown</baitSchedule>
            <baitNotes>none</baitNotes>
            <cameraDeploymentNotes>none</cameraDeploymentNotes>
            <collector>unknown</collector>
            <cameraDeployId><xsl:value-of select="$label"/></cameraDeployId>
        </cameraDeployment>
    </xsl:template>
</xsl:stylesheet>
