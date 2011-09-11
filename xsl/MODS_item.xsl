<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
    <xsl:stylesheet version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:fmp="http://www.filemaker.com/fmpdsoresult"
        exclude-result-prefixes="fmp">
   <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
       
        <xsl:template match="/">
                
            <xsl:for-each select="//fmp:ROW">
                
                <!--<xsl:call-template name="control"/>   -->            
                <xsl:call-template name="main"/>      
                       
            </xsl:for-each>
               
        </xsl:template>
        
      
       <!-- MODS-->
        <xsl:template name="main" >
            <mods xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3" xsi:schemaLocation="http://www.loc.gov/mods/v3 http://www.loc.gov/standards/mods/v3/mods-3-4.xsd">
            <titleInfo>
                <xsl:for-each select="fmp:i_title/fmp:DATA">
                <title><xsl:value-of select="."/></title>
                </xsl:for-each>
                <xsl:for-each select="fmp:i_alt_title/fmp:DATA">
                    <altTitle><xsl:value-of select="."/></altTitle>
                </xsl:for-each>
            </titleInfo>
            
            <xsl:for-each select="fmp:i_creator/fmp:DATA">
                <name type="personal">
                    <role>
                        <roleTerm authority="marcrelator" type="text">creator</roleTerm>
                    </role>
                    <namePart><xsl:value-of select="."/></namePart>
                </name>
            </xsl:for-each>
            <xsl:for-each select="fmp:i_overall_condition">
                <note type="conservationHistory"><xsl:value-of select="."/></note>
            </xsl:for-each>
            <xsl:for-each select="fmp:i_overall_condition | fmp:i_current_enclosure | fmp:i_cataloger_notes">
                <note xmlns="http://www.loc.gov/mods/v3" type="conservationHistory"><xsl:value-of select="."/></note>
                </xsl:for-each>
            <xsl:for-each select="fmp:i_abstract">
                <abstract  ><xsl:value-of select="."/></abstract>
            </xsl:for-each>    
            <xsl:for-each select="fmp:itemID">
                <identifier  ><xsl:value-of select="."/></identifier>
            </xsl:for-each> 
            <recordInfo>            
            <xsl:for-each select="fmp:i_record_origin">
                <recordOrigin ><xsl:value-of select="."/></recordOrigin>
            </xsl:for-each>
            <xsl:for-each select="fmp:i_record_content_source">
                <recordContentSource  ><xsl:value-of select="."/></recordContentSource>
            </xsl:for-each>
                <xsl:for-each select="fmp:i_record_creation_date">
                    <recordCreationDate  ><xsl:value-of select="."/></recordCreationDate>
                </xsl:for-each>
                <xsl:for-each select="i_record_change_date">
                    <recordChangeDate  ><xsl:value-of select="."/></recordChangeDate>
                </xsl:for-each>
            </recordInfo>
            <location >
            <xsl:for-each select="fmp:i_location">
                <physicalLocation ><xsl:value-of select="."/></physicalLocation>
            </xsl:for-each>
            <xsl:for-each select="fmp:i_sublocation">
                <holdingSimple > <copyInformation><subLocation><xsl:value-of select="."/></subLocation></copyInformation></holdingSimple>
            </xsl:for-each>
                <xsl:for-each select="fmp:i_shelf_locator"><shelfLocator><xsl:value-of select="."/></shelfLocator></xsl:for-each>
            </location>
            <originInfo >
                <xsl:for-each select="fmp:i_date_range">
                    <dateOther><xsl:value-of select="."/></dateOther>
                </xsl:for-each>
            </originInfo>
            <xsl:for-each select="fmp:i_note">
                <note  ><xsl:value-of select="."/></note>
            </xsl:for-each>
            <xsl:for-each select="i_resource_type">
                <typeOfResource xmlns="http://www.loc.gov/mods/v3" ><xsl:value-of select="."/></typeOfResource>
            </xsl:for-each>
            <xsl:for-each select="i_physical_description">
                <physicalDescription xmlns="http://www.loc.gov/mods/v3" ><form><xsl:value-of select="."/></form></physicalDescription>
            </xsl:for-each>
            <xsl:for-each select="i_access_condition"><accessCondition ><xsl:value-of select="."/></accessCondition></xsl:for-each>
            <xsl:for-each select="i_date1 | i_date2">
                <dateCreated><xsl:value-of select="."/></dateCreated>
            </xsl:for-each>
            <xsl:for-each select="fmp:i_genre/fmp:DATA">   
                <xsl:if test="./text()">                    
                    <xsl:variable name="SUBJECT" select="."/>                               
                    <xsl:variable name="VOC" select="substring-after($SUBJECT,' }} ')"/>    
                    <xsl:variable name="TERM" select="substring-before($SUBJECT, ' }} ')"/>                      
                    <genre authority="{$VOC}"><xsl:value-of select="$TERM"/></genre>
                </xsl:if>
            </xsl:for-each>      
            <xsl:for-each select="fmp:i_subject_topic/fmp:DATA">   
                <xsl:if test="./text()">
                    <subject>
                        <xsl:variable name="SUBJECT" select="."/>                               
                        <xsl:variable name="VOC" select="substring-after($SUBJECT,' }} ')"/>    
                        <xsl:variable name="TERM" select="substring-before($SUBJECT, ' }} ')"/>                      
                        <topic authority="{$VOC}"><xsl:value-of select="$TERM"/></topic></subject>
                </xsl:if>
            </xsl:for-each>    
            <xsl:for-each select="fmp:i_subject_geographic/fmp:DATA">   
                <xsl:if test="./text()">
                    <subject>
                        <xsl:variable name="SUBJECT" select="."/>                               
                        <xsl:variable name="VOC" select="substring-after($SUBJECT,' }} ')"/>    
                        <xsl:variable name="TERM" select="substring-before($SUBJECT, ' }} ')"/>                      
                        <geographic authority="{$VOC}"><xsl:value-of select="$TERM"/></geographic></subject>
                </xsl:if>
            </xsl:for-each> 
                <xsl:for-each select="fmp:i_subject_person/fmp:DATA">   
                    <xsl:if test="./text()">
                        <subject>
                            <xsl:variable name="SUBJECT" select="."/>                               
                            <xsl:variable name="VOC" select="substring-after($SUBJECT,' }} ')"/>    
                            <xsl:variable name="TERM" select="substring-before($SUBJECT, ' }} ')"/>                      
                            <topic authority="{$VOC}"><xsl:value-of select="$TERM"/></topic></subject>
                    </xsl:if>
                </xsl:for-each> 
                <xsl:if test="i_language1">
            <language>
            <xsl:for-each select="i_language1"><languageTerm><xsl:value-of select="."/><authority><xsl:value-of select="i_language1_code"/></authority></languageTerm></xsl:for-each>
                <xsl:for-each select="i_language1_script"><scriptTerm><xsl:value-of select="."/><authority><xsl:value-of select="i_language1_script_code"/></authority></scriptTerm></xsl:for-each>
            </language>
                </xsl:if>
            </mods>            
        </xsl:template>
       
</xsl:stylesheet>