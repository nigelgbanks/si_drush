<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
<xsl:stylesheet version="1.0" xmlns="urn:isbn:1-931666-33-4"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="eac">
    <xsl:import href="EAC_Base.xsl"/>
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <!-- Entry Point -->
    <xsl:template match="/">
        <xsl:for-each select="//eac:ROW">
            <xsl:call-template name="eaccpf">
                <xsl:with-param name="id" select="eac:organizationID"/>
                <xsl:with-param name="type">corporateBody</xsl:with-param>
                <xsl:with-param name="maintenance_status" select="eac:o_maintenanceStatus"/>
                <xsl:with-param name="publication_status" select="eac:o_publicationStatus"/>
                <xsl:with-param name="agency" select="eac:o_maintenance_agency"/>
                <xsl:with-param name="created" select="eac:o_eventDateTime1"/>
                <xsl:with-param name="edited" select="eac:o_eventDateTime2"/>
                <xsl:with-param name="harvested" select="eac:o_eventDateTime3"/>
                <xsl:with-param name="sources"
                    select="eac:o_source/eac:DATA[normalize-space(text())]"/>
                <xsl:with-param name="primary_name" select="eac:o_primary_name"/>
                <xsl:with-param name="abbrivated_name" select="eac:org_abbreviation"/>
                <xsl:with-param name="alternative_names"
                    select="eac:o_alt_name1 | eac:o_alt_name3 | eac:o_alt_name3"/>
                <xsl:with-param name="description" select="eac:o_description"/>
                <xsl:with-param name="languages" select="eac:o_language1 | eac:o_language2 | eac:o_language3"/>
                <xsl:with-param name="language_codes"
                    select="eac:o_language1_code | eac:o_language2_code | eac:o_language3_code"/>
                <xsl:with-param name="scripts"
                    select="eac:o_language1_script | eac:o_language2_script | eac:o_language3_script"/>
                <xsl:with-param name="script_codes"
                    select="eac:o_language1_script_code | eac:o_language2_script_code | eac:o_language3_script_code"/>
                <xsl:with-param name="address_line1" select="eac:o_address1"/>
                <xsl:with-param name="address_line2" select="eac:o_address2"/>
                <xsl:with-param name="city" select="eac:o_city"/>
                <xsl:with-param name="state" select="eac:o_state"/>
                <xsl:with-param name="postal_code" select="eac:o_state"/>
                <xsl:with-param name="country" select="eac:o_country"/>
                <xsl:with-param name="url" select="eac:o_URL"/>
                <xsl:with-param name="phone" select="eac:o_phone"/>
                <xsl:with-param name="email_address" select="eac:o_email_address"/>
                <xsl:with-param name="start_date" select="eac:exist_date1"/>
                <xsl:with-param name="end_date" select="eac:exist_date2"/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:template>
    <!-- EAC relationships -->
    <xsl:template name="relations">
        <xsl:variable name="parent_org" select="normalize-space(eac:o_parent_org)"/>
        <relations>
            <xsl:if test="$parent_org">
                <xsl:call-template name="cpfRelation">
                    <xsl:with-param name="type">hierarchical-parent</xsl:with-param>
                    <xsl:with-param name="localType">parent organization</xsl:with-param>
                    <xsl:with-param name="value" select="$parent_org"/>
                </xsl:call-template>
            </xsl:if>
        </relations>
    </xsl:template>
</xsl:stylesheet>