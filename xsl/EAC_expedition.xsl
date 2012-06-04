<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
<xsl:stylesheet version="1.0"
    xmlns="urn:isbn:1-931666-33-4"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="eac">
    <xsl:import href="EAC_Base.xsl"/>
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <!-- Entry Point -->
    <xsl:template match="/">
        <xsl:for-each select="//eac:ROW">
            <xsl:call-template name="eaccpf">
                <xsl:with-param name="id" select="eac:expeditionID"/>
                <xsl:with-param name="type">corporateBody</xsl:with-param>
                <xsl:with-param name="maintenance_status" select="eac:e_maintenanceStatus"/>
                <xsl:with-param name="publication_status" select="eac:e_publicationStatus"/>
                <xsl:with-param name="agency" select="eac:e_maintenance_agency"/>
                <xsl:with-param name="created" select="eac:e_eventDateTime1"/>
                <xsl:with-param name="edited" select="eac:e_eventDateTime2"/>
                <xsl:with-param name="harvested" select="eac:e_eventDateTime3"/>
                <xsl:with-param name="sources" select="eac:e_source/eac:DATA[normalize-space(text())]"/>
                <xsl:with-param name="primary_name" select="eac:e_primary_name"/>
                <xsl:with-param name="alternative_names" select="eac:e_alt_name1 | eac:e_alt_name3 | eac:e_alt_name3"/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:template>
    <!-- Description -->
    <xsl:template name="description">
        <xsl:variable name="start_date" select="eac:e_exist_date_start"/>
        <xsl:variable name="end_date" select="eac:e_exist_date_end"/>
        <xsl:variable name="exist_date_description" select="eac:e_exist_date_description"/>
        <xsl:variable name="languages" select="eac:e_language1 | eac:e_language2"/>
        <xsl:variable name="description" select="eac:e_description"/>
        <xsl:variable name="language_codes" select="eac:e_language1_code | eac:e_language2_code"/>
        <xsl:variable name="scripts" select="eac:e_language1_script | eac:e_language2_script"/>
        <xsl:variable name="script_codes" select="eac:e_language1_script_code | eac:e_language2_script_code"/>
        <xsl:variable name="subjects" select="eac:e_subject/eac:DATA[normalize-space(text())]"/>
        <xsl:variable name="places" select="eac:e_place/eac:DATA[normalize-space(text())]"/>
        <description>
            <xsl:call-template name="existDates">
                <xsl:with-param name="start_date" select="$start_date"/>
                <xsl:with-param name="end_date" select="$end_date"/>
                <xsl:with-param name="exist_date_description" select="$exist_date_description"/>
            </xsl:call-template>
            <xsl:call-template name="localDescriptions">
                <xsl:with-param name="type">subjects</xsl:with-param>
                <xsl:with-param name="localType">subject</xsl:with-param>
                <xsl:with-param name="terms" select="$subjects"/>
            </xsl:call-template>
            <xsl:call-template name="localDescriptions">
                <xsl:with-param name="type">places</xsl:with-param>
                <xsl:with-param name="localType">place</xsl:with-param>
                <xsl:with-param name="terms" select="$places"/>
            </xsl:call-template>
            <xsl:call-template name="languagesUsed">
                <xsl:with-param name="languages" select="$languages"/>
                <xsl:with-param name="language_codes" select="$language_codes"/>
                <xsl:with-param name="scripts" select="$scripts"/>
                <xsl:with-param name="script_codes" select="$script_codes"/>
            </xsl:call-template>
            <xsl:call-template name="biogHist">
                <xsl:with-param name="description" select="$description"/>
            </xsl:call-template>
        </description>
    </xsl:template>
    <!-- EAC relationships -->
    <xsl:template name="relations">
        <xsl:variable name="participants" select="eac:e_participant_person/eac:DATA[normalize-space(text())]"/>
        <xsl:variable name="organizations" select="eac:e_participant_organization/eac:DATA[normalize-space(text())]"/>
        <xsl:variable name="materials" select="eac:e_related_material/eac:DATA[normalize-space(text())]"/>
        <relations>
            <xsl:for-each select="$participants">
                <xsl:call-template name="cpfRelation">
                    <xsl:with-param name="type">associative</xsl:with-param>
                    <xsl:with-param name="localType">person</xsl:with-param>
                    <xsl:with-param name="value" select="."/>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:for-each select="$organizations">
                <xsl:call-template name="cpfRelation">
                    <xsl:with-param name="type">associative</xsl:with-param>
                    <xsl:with-param name="localType">organization</xsl:with-param>
                    <xsl:with-param name="value" select="."/>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:for-each select="$materials">
                <xsl:call-template name="resourceRelation">
                    <xsl:with-param name="type">other</xsl:with-param>
                    <xsl:with-param name="localType">material</xsl:with-param>
                    <xsl:with-param name="value" select="."/>
                </xsl:call-template>
            </xsl:for-each>
        </relations>
    </xsl:template>
</xsl:stylesheet>
