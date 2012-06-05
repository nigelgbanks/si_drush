<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
<xsl:stylesheet version="1.0"
    xmlns="http://www.loc.gov/mods/v3"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:fmp="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="fmp">
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <!-- Entry Point -->
    <xsl:template match="/">
        <xsl:for-each select="//fmp:ROW">
            <xsl:call-template name="mods"/>
        </xsl:for-each>
    </xsl:template>
    <!-- MODS-->
    <xsl:template name="mods">
        <mods xmlns="http://www.loc.gov/mods/v3"
            xmlns:mods="http://www.loc.gov/mods/v3"
            xmlns:xlink="http://www.w3.org/1999/xlink"
            xsi:schemaLocation="http://www.loc.gov/mods/v3 http://www.loc.gov/standards/mods/v3/mods-3-4.xsd" ID="{fmp:itemID}">
            <abstract>
                <xsl:value-of select="fmp:i_abstract"/>
            </abstract>
            <xsl:call-template name="titleInfo">
                <xsl:with-param name="type" select="fmp:i_title_type"/>
                <xsl:with-param name="title" select="fmp:i_title/descendant-or-self::node()[text() != '' and not(child::*)]"/>
                <xsl:with-param name="href" select="fmp:i_href"/>
            </xsl:call-template>
            <xsl:call-template name="titleInfo">
                <xsl:with-param name="type">alternative</xsl:with-param>
                <xsl:with-param name="title" select="fmp:i_alt_title"/>
            </xsl:call-template>
            <xsl:for-each select="fmp:i_creator/fmp:DATA[normalize-space(text())]">
                <xsl:call-template name="name">
                    <xsl:with-param name="name" select="."/>
                    <xsl:with-param name="type">personal</xsl:with-param>
                    <xsl:with-param name="role">creator</xsl:with-param>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:for-each select="fmp:i_contributor/fmp:DATA[normalize-space(text())]">
                <xsl:call-template name="name">
                    <xsl:with-param name="name" select="."/>
                    <xsl:with-param name="type">personal</xsl:with-param>
                    <xsl:with-param name="role">contributor</xsl:with-param>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:call-template name="identifier">
                <xsl:with-param name="value" select="fmp:itemID"/>
            </xsl:call-template>
            <xsl:call-template name="identifier">
                <xsl:with-param name="type">accessionNumber</xsl:with-param>
                <xsl:with-param name="value" select="fmp:i_altID"/>
            </xsl:call-template>
            <xsl:for-each select="fmp:i_expedition_name/fmp:DATA[normalize-space(text())]">
                <xsl:call-template name="identifier">
                    <xsl:with-param name="type">expedition</xsl:with-param>
                    <xsl:with-param name="value" select="text()"/>
                </xsl:call-template>    
            </xsl:for-each>
            <xsl:call-template name="note">
                <xsl:with-param name="value" select="fmp:i_note"/>
            </xsl:call-template>
            <xsl:for-each select="fmp:i_vessel_name/fmp:DATA[normalize-space(text())]">
                <xsl:call-template name="note">
                    <xsl:with-param name="type">vessel</xsl:with-param>
                    <xsl:with-param name="value" select="text()"/>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:call-template name="note">
                <xsl:with-param name="type">href</xsl:with-param>
                <xsl:with-param name="value" select="fmp:i_href"/>
            </xsl:call-template>            
            <xsl:call-template name="recordInfo"/>
            <xsl:call-template name="location"/>
            <xsl:call-template name="originInfo"/>
            <xsl:call-template name="typeOfResource"/>
            <physicalDescription>
                <form>
                    <xsl:value-of select="fmp:i_physical_description"/>
                </form>
            </physicalDescription>
            <accessCondition>
                <xsl:value-of select="fmp:i_access_condition"/>
            </accessCondition>
            <xsl:if test="normalize-space(fmp:i_collection)">
                <relatedItem type="host" ID="{fmp:i_collection}"/>
            </xsl:if>
            <xsl:for-each select="fmp:i_related_item/fmp:DATA[normalize-space(text())]">
                <relatedItem>
                    <xsl:call-template name="titleInfo">
                        <xsl:with-param name="title" select="text()"/>
                    </xsl:call-template>
                </relatedItem>
            </xsl:for-each>
            <xsl:call-template name="genres"/>
            <xsl:call-template name="subjects">
                <xsl:with-param name="subjects" select="fmp:i_subject_topic/fmp:DATA[normalize-space(text())]"/>
                <xsl:with-param name="type">topic</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="subjects">
                <xsl:with-param name="subjects" select="fmp:i_subject_geographical/fmp:DATA[normalize-space(text())]"/>
                <xsl:with-param name="type">geographic</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="subjects">
                <xsl:with-param name="subjects" select="fmp:i_subject_person/fmp:DATA[normalize-space(text())]"/>
                    <xsl:with-param name="type">person</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="subjects">
                <xsl:with-param name="subjects" select="fmp:i_subject_organization/fmp:DATA[normalize-space(text())]"/>
                <xsl:with-param name="type">organization</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="langauges">
                <xsl:with-param name="languages" select="fmp:i_language1 | fmp:i_language2"/>
                <xsl:with-param name="language_codes" select="fmp:i_language1_code | fmp:i_language2_code"/>
                <xsl:with-param name="scripts" select="fmp:i_language1_script | fmp:i_language2_script"/>
                <xsl:with-param name="script_codes" select="fmp:i_language1_script_code | fmp:i_language2_script_code"/>
            </xsl:call-template>
        </mods>
    </xsl:template>
    <!-- Title Info -->
    <xsl:template name="titleInfo">
        <xsl:param name="type"/>
        <xsl:param name="title"/>
        <titleInfo>
            <xsl:call-template name="titleType">
                <xsl:with-param name="value" select="$type"/>
            </xsl:call-template>
            <title>
                <xsl:value-of select="$title"/>
            </title>
        </titleInfo>
    </xsl:template>
    <!-- Title Type -->
    <xsl:template name="titleType">
        <xsl:param name="value"/>
        <xsl:choose>
            <xsl:when test="$value = 'abbreviated'">
                <xsl:attribute name="type">abbreviated</xsl:attribute>
            </xsl:when>
            <xsl:when test="$value = 'translated'">
                <xsl:attribute name="type">translated</xsl:attribute>
            </xsl:when>
            <xsl:when test="$value = 'alternative'">
                <xsl:attribute name="type">alternative</xsl:attribute>
            </xsl:when>
            <xsl:when test="$value = 'uniform'">
                <xsl:attribute name="type">uniform</xsl:attribute>
            </xsl:when>
        </xsl:choose>
    </xsl:template>
    <!-- Name -->
    <xsl:template name="name">
        <xsl:param name="name"/>
        <xsl:param name="type"/>
        <xsl:param name="role"/>
        <name type="personal">
            <xsl:call-template name="nameType">
                <xsl:with-param name="value" select="$type"/>
            </xsl:call-template>
            <role>
                <roleTerm authority="marcrelator" type="text">
                    <xsl:value-of select="$role"/>
                </roleTerm>
            </role>
            <namePart>
                <xsl:value-of select="$name"/>
            </namePart>
        </name>
    </xsl:template>
    <!-- Name Type -->
    <xsl:template name="nameType">
        <xsl:param name="value"/>
        <xsl:choose>
            <xsl:when test="$value = 'personal'">
                <xsl:attribute name="type">personal</xsl:attribute>
            </xsl:when>
            <xsl:when test="$value = 'corporate'">
                <xsl:attribute name="type">corporate</xsl:attribute>
            </xsl:when>
            <xsl:when test="$value = 'conference'">
                <xsl:attribute name="type">conference</xsl:attribute>
            </xsl:when>
            <xsl:when test="$value = 'family'">
                <xsl:attribute name="type">family</xsl:attribute>
            </xsl:when>
        </xsl:choose>
    </xsl:template>
    <!-- Note -->
    <xsl:template name="note">
        <xsl:param name="type"/>
        <xsl:param name="value"/>
        <xsl:element name="note">
            <xsl:if test="$type">
                <xsl:attribute name="type">
                    <xsl:value-of select="$type"/>
                </xsl:attribute>
            </xsl:if>
            <xsl:value-of select="$value"/>
        </xsl:element>
    </xsl:template>
    <!-- Identifier -->
    <xsl:template name="identifier">
        <xsl:param name="type"/>
        <xsl:param name="value"/>
        <xsl:if test="$value">
            <identifier>
                <xsl:if test="$type">
                    <xsl:attribute name="type">
                        <xsl:value-of select="$type"/>
                    </xsl:attribute>
                </xsl:if>
                <xsl:value-of select="$value"/>
            </identifier>    
        </xsl:if>
    </xsl:template>
    <!-- Date Qualifier -->
    <xsl:template name="dateQualifier">
        <xsl:param name="value"/>
        <xsl:choose>
            <xsl:when test="$value = 'approximate'">
                <xsl:attribute name="qualifier">approximate</xsl:attribute>        
            </xsl:when>
            <xsl:when test="$value = 'inferred'">
                <xsl:attribute name="qualifier">inferred</xsl:attribute>        
            </xsl:when>
            <xsl:when test="$value = 'questionable'">
                <xsl:attribute name="qualifier">questionable</xsl:attribute>        
            </xsl:when>
        </xsl:choose>
    </xsl:template>
    <!-- Type Of Resource -->
    <xsl:template name="typeOfResource">
        <xsl:variable name="value" select="fmp:i_resource_type"/>
        <typeOfResource>
            <xsl:choose>
                <xsl:when test="$value = 'text'">
                    <xsl:text>text</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'cartographic'">
                    <xsl:text>cartographic</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'notated music'">
                    <xsl:text>notated music</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'sound recording-musical'">
                    <xsl:text>sound recording-musical</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'sound recording-nonmusical'">
                    <xsl:text>sound recording-nonmusical</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'sound recording'">
                    <xsl:text>sound recording</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'still image'">
                    <xsl:text>still image</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'moving image'">
                    <xsl:text>moving image</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'three dimensional object'">
                    <xsl:text>three dimensional object</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'software, multimedia'">
                    <xsl:text>software, multimedia</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'mixed material'">
                    <xsl:text>mixed material</xsl:text>
                </xsl:when>
                <xsl:otherwise></xsl:otherwise>
            </xsl:choose>
        </typeOfResource>
    </xsl:template>
    <!-- Location -->
    <xsl:template name="location">
        <location>
            <physicalLocation>
                <xsl:value-of select="fmp:i_location"/>
            </physicalLocation>
            <shelfLocator>
                <xsl:value-of select="fmp:i_shelf_locator"/>
            </shelfLocator>
            <url>
                <xsl:value-of select="fmp:i_physical_location_website"/>
            </url>
            <holdingSimple>
                <copyInformation>
                    <subLocation>
                        <xsl:value-of select="fmp:i_sublocation"/>
                    </subLocation>
                </copyInformation>
            </holdingSimple>
        </location>
    </xsl:template>
    <!-- Record Info -->
    <xsl:template name="recordInfo">
        <recordInfo>
            <recordOrigin>
                <xsl:value-of select="fmp:i_record_origin"/>
            </recordOrigin>
            <recordContentSource>
                <xsl:value-of select="fmp:i_record_content_source"/>
            </recordContentSource>
            <recordCreationDate>
                <xsl:value-of select="fmp:i_record_creation_date"/>
            </recordCreationDate>
            <recordChangeDate>
                <xsl:value-of select="fmp:i_record_change_date"/>
            </recordChangeDate>
        </recordInfo>
    </xsl:template>
    <!-- Origin Info -->
    <xsl:template name="originInfo">
        <xsl:variable name="qualifier" select="fmp:i_date_qualifier"/>
        <originInfo>
            <dateOther type="range">
                <xsl:call-template name="dateQualifier">
                    <xsl:with-param name="value" select="$qualifier"/>
                </xsl:call-template>
                <xsl:value-of select="fmp:i_date_range"/>
            </dateOther>
            <dateOther type="range" point="start">
                <xsl:call-template name="dateQualifier">
                    <xsl:with-param name="value" select="$qualifier"/>
                </xsl:call-template>
                <xsl:value-of select="fmp:i_date1"/>
            </dateOther>
            <dateOther type="range" point="end">
                <xsl:call-template name="dateQualifier">
                    <xsl:with-param name="value" select="$qualifier"/>
                </xsl:call-template>
                <xsl:value-of select="fmp:i_date2"/>
            </dateOther>
        </originInfo>
    </xsl:template>
    <!-- Genres -->
    <xsl:template name="genres">
        <xsl:for-each select="fmp:i_genre/fmp:DATA[normalize-space(text())]">
            <xsl:call-template name="genre">
                <xsl:with-param name="value">
                    <xsl:call-template name="getTerm">
                        <xsl:with-param name="input" select="text()"/>
                        <xsl:with-param name="index" select="1"/>
                    </xsl:call-template>
                </xsl:with-param>
                <xsl:with-param name="authority">
                    <xsl:call-template name="getTerm">
                        <xsl:with-param name="input" select="text()"/>
                        <xsl:with-param name="index" select="2"/>
                    </xsl:call-template>
                </xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:template>
    <!-- Genre -->
    <xsl:template name="genre">
        <xsl:param name="value"/>
        <xsl:param name="authority"/>
        <genre authority="{$authority}">
            <xsl:value-of select="$value"/>
        </genre>
    </xsl:template>
    <!-- Genres -->
    <xsl:template name="subjects">
        <xsl:param name="subjects"/>
        <xsl:param name="type"/>
        <xsl:for-each select="$subjects">
            <xsl:call-template name="subject">
                <xsl:with-param name="type" select="$type"/>
                <xsl:with-param name="value">
                    <xsl:call-template name="getTerm">
                        <xsl:with-param name="input" select="text()"/>
                        <xsl:with-param name="index" select="1"/>
                    </xsl:call-template>
                </xsl:with-param>
                <xsl:with-param name="authority">
                    <xsl:call-template name="getTerm">
                        <xsl:with-param name="input" select="text()"/>
                        <xsl:with-param name="index" select="2"/>
                    </xsl:call-template>
                </xsl:with-param>
                <xsl:with-param name="authority_id">
                    <xsl:call-template name="getTerm">
                        <xsl:with-param name="input" select="text()"/>
                        <xsl:with-param name="index" select="3"/>
                    </xsl:call-template>
                </xsl:with-param>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:template>
    <!-- Subject -->
    <xsl:template name="subject">
        <xsl:param name="type"/>
        <xsl:param name="value"/>
        <xsl:param name="authority"/>
        <xsl:param name="authority_id"/>
        <subject>
            <xsl:choose>
                <xsl:when test="$type = 'topic'">
                    <topic authority="{$authority}">
                        <xsl:value-of select="$value"/>
                    </topic>
                </xsl:when>
                <xsl:when test="$type = 'geographic'">
                    <geographic authority="{$authority}">
                        <xsl:if test="normalize-space($authority_id)">
                            <xsl:attribute name="authorityURI">
                                <xsl:value-of select="$authority_id"/>
                            </xsl:attribute>
                        </xsl:if>
                        <xsl:value-of select="$value"/>
                    </geographic>
                </xsl:when>
                <xsl:when test="$type = 'person'">
                    <name type="personal">
                        <namePart>
                            <xsl:value-of select="$value"/>    
                        </namePart>
                    </name>
                </xsl:when>
                <xsl:when test="$type = 'organization'">
                    <name type="corporate">
                        <namePart>
                            <xsl:value-of select="$value"/>    
                        </namePart>
                    </name>
                </xsl:when>
                <xsl:when test="$type = 'vessel'">
                    <name type="corporate">
                        <namePart>
                            <xsl:value-of select="$value"/>    
                        </namePart>
                    </name>
                </xsl:when>
            </xsl:choose>
        </subject>
    </xsl:template>
    <!-- Languages -->
    <xsl:template name="langauges">
        <xsl:param name="languages"/>
        <xsl:param name="language_codes"/>
        <xsl:param name="scripts"/>
        <xsl:param name="script_codes"/>
        <xsl:for-each select="$languages[normalize-space(text())]">
            <xsl:call-template name="language">
                <xsl:with-param name="language" select="."/>
                <xsl:with-param name="language_code" select="$language_codes[position()]"/>
                <xsl:with-param name="script" select="$scripts[position()]"/>
                <xsl:with-param name="script_code" select="$script_codes[position()]"/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:template>
    <!-- Language -->
    <xsl:template name="language">
        <xsl:param name="language"/>
        <xsl:param name="language_code"/>
        <xsl:param name="script"/>
        <xsl:param name="script_code"/>
        <language>
            <languageTerm type="code">
                <xsl:value-of select="$language_code"/>
            </languageTerm>
            <languageTerm type="text">
                <xsl:value-of select="$language"/>
            </languageTerm>
            <scriptTerm type="code">
                <xsl:value-of select="$script_code"/>    
            </scriptTerm>
            <scriptTerm type="text">
                <xsl:value-of select="$script"/>    
            </scriptTerm>
        </language>
    </xsl:template>
    <!--
        getTerm:
         input - Delimited String to parse 
         index - The index of the delimited field to return, 1 based. 
         delimiter - The delimiting text that seperates the fields         
    -->
    <xsl:template name="getTerm">
        <xsl:param name="input"/>
        <xsl:param name="index" select="0"/>
        <xsl:param name="delimiter"> }} </xsl:param>
        <xsl:variable name="before" select="substring-before($input, $delimiter)"/>
        <xsl:variable name="after" select="substring-after($input, $delimiter)"/>
        <xsl:if test="$index = 1">
            <xsl:if test="$before = ''">
                <xsl:value-of select="$input"/>
            </xsl:if>
            <xsl:if test="$before != ''">
                <xsl:value-of select="$before"/>
            </xsl:if>
        </xsl:if>
        <xsl:if test="$index > 1">
            <xsl:call-template name="getTerm">
                <xsl:with-param name="input" select="$after"/>
                <xsl:with-param name="index" select="$index -1"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
</xsl:stylesheet>
