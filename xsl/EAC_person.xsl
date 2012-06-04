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
                <xsl:with-param name="id" select="eac:personID"/>
                <xsl:with-param name="type">person</xsl:with-param>
                <xsl:with-param name="maintenance_status" select="eac:p_maintenanceStatus"/>
                <xsl:with-param name="publication_status" select="eac:p_publicationStatus"/>
                <xsl:with-param name="agency" select="eac:p_maintenance_agency"/>
                <xsl:with-param name="created" select="eac:p_eventDateTime1"/>
                <xsl:with-param name="edited" select="eac:p_eventDateTimet2"/>
                <xsl:with-param name="harvested" select="eac:p_eventDateTime3"/>
                <xsl:with-param name="sources"
                    select="eac:p_source/eac:DATA[normalize-space(text())]"/>
                <xsl:with-param name="primary_name" select="eac:p_primary_name"/>
                <xsl:with-param name="alternative_names"
                    select="eac:p_alt_name1 | eac:p_alt_name3 | eac:p_alt_name3"/>
                <xsl:with-param name="description" select="eac:p_bio_history"/>
                <xsl:with-param name="languages" select="eac:p_language1 | eac:p_language2 | eac:p_language3"/>
                <xsl:with-param name="language_codes"
                    select="eac:p_language1_code | eac:p_language2_code | eac:p_language3_code"/>
                <xsl:with-param name="scripts"
                    select="eac:p_language1_script | eac:p_language2_script | eac:p_language3_script"/>
                <xsl:with-param name="script_codes"
                    select="eac:p_language1_script_code | eac:p_language2_script_code | eac:p_language3_script_code"/>
                <xsl:with-param name="address_line1" select="eac:p_address1"/>
                <xsl:with-param name="address_line2" select="eac:p_address2"/>
                <xsl:with-param name="city" select="eac:p_city"/>
                <xsl:with-param name="state" select="eac:p_state"/>
                <xsl:with-param name="postal_code" select="eac:p_state"/>
                <xsl:with-param name="country" select="eac:p_country"/>
                <xsl:with-param name="url" select="eac:p_URL"/>
                <xsl:with-param name="phone" select="eac:p_phone"/>
                <xsl:with-param name="email_address" select="eac:p_email_address"/>
                <xsl:with-param name="start_date" select="eac:p_exist_date_b"/>
                <xsl:with-param name="end_date" select="eac:p_exist_date_d"/>
                <xsl:with-param name="occupations" select="eac:p_occupation/eac:DATA[normalize-space(text())]"/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:template>
    <!-- Identity -->
    <xsl:template name="identity">
        <xsl:param name="id"/>
        <xsl:param name="type"/>
        <xsl:param name="primary_name"/>
        <xsl:param name="alternative_names"/>
        <identity>
            <xsl:call-template name="entityId">
                <xsl:with-param name="id" select="$id"/>
            </xsl:call-template>
            <xsl:call-template name="entityType">
                <xsl:with-param name="type" select="$type"/>
            </xsl:call-template>
            <xsl:call-template name="primaryName">
                <xsl:with-param name="primary_name" select="$primary_name"/>
            </xsl:call-template>
            <xsl:call-template name="fullName"/>
            <xsl:call-template name="alternativeNames">
                <xsl:with-param name="alternative_names" select="$alternative_names"/>
            </xsl:call-template>
        </identity>
    </xsl:template>
    <!-- Full Name -->
    <xsl:template name="fullName">
        <nameEntry localType="full">
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
    </xsl:template>
    <!-- EAC relationships -->
    <xsl:template name="relations">
        <xsl:variable name="entity1" select="normalize-space(eac:p_related_entity1)"/>
        <xsl:variable name="entity2" select="normalize-space(eac:p_related_entity2)"/>
        <xsl:variable name="entity3" select="normalize-space(eac:p_related_entity3)"/>
        <xsl:variable name="materials" select="eac:p_related_material/eac:DATA[normalize-space(text())]"/>
        <relations>
            <xsl:if test="$entity1">
                <xsl:call-template name="cpfRelation">
                    <xsl:with-param name="type">associative</xsl:with-param>
                    <xsl:with-param name="localType">undefined</xsl:with-param>
                    <xsl:with-param name="value" select="$entity1"/>
                    <xsl:with-param name="description" select="eac:p_related_entity1_description"/>
                </xsl:call-template>
            </xsl:if>
            <xsl:if test="$entity2">
                <xsl:call-template name="cpfRelation">
                    <xsl:with-param name="type">associative</xsl:with-param>
                    <xsl:with-param name="localType">undefined</xsl:with-param>
                    <xsl:with-param name="value" select="$entity2"/>
                    <xsl:with-param name="description" select="eac:p_related_entity2_description"/>
                </xsl:call-template>
            </xsl:if>
            <xsl:if test="$entity3">
                <xsl:call-template name="cpfRelation">
                    <xsl:with-param name="type">associative</xsl:with-param>
                    <xsl:with-param name="localType">undefined</xsl:with-param>
                    <xsl:with-param name="value" select="$entity3"/>
                    <xsl:with-param name="description" select="eac:p_related_entity3_description"/>
                </xsl:call-template>
            </xsl:if>
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
