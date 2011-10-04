<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
    <xsl:stylesheet version="1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:fmp="http://www.filemaker.com/fmpdsoresult"
        exclude-result-prefixes="fmp">
   <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
       
        <xsl:template match="/">
                
            <xsl:for-each select="//fmp:ROW">
                <RecordSet xmlns="http://rs.tdwg.org/ncd/0.70"
                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                    xmlns:xlink="http://www.w3.org/1999/xlink"
                    xsi:schemaLocation="http://rs.tdwg.org/ncd/0.70 file:/Users/paulpound/Library/Mail%20Downloads/ncdLite_v1_2-1.xsd">
                    <Collections>
                        <Collection id="{fmp:collectionID}">
                            <CollectionId><xsl:value-of select="fmp:collectionID"/></CollectionId>
                            <AlternativeIds><Identifier id="{fmp:c_altID}" source="Smithsonian"/></AlternativeIds>
                            <AboutThisRecord>
                                <dc_source><xsl:value-of select="fmp:c_corporate_affiliation"/></dc_source>
                                <dc_identifier><xsl:value-of select="fmp:collectionID"/></dc_identifier>
                                <dct_created><xsl:value-of select="fmp:c_record_created_date"/></dct_created>
                                <dct_creator><xsl:value-of select="fmp:c_author"/></dct_creator>
                                <dct_modified><xsl:value-of select="fmp:c_record_edited_date"/></dct_modified>
                                <dct_harvested><xsl:value-of select="fmp:c_record_harvest_date"/></dct_harvested>
                            </AboutThisRecord>            
                            <DesciptiveGroup>
                                <dc_title><xsl:value-of select="fmp:c_primary_name"/></dc_title>
                                <dc_alternative_title><xsl:value-of select="fmp:c_alt_name"/></dc_alternative_title>
                                <dc_description><xsl:value-of select="fmp:c_descr"/></dc_description>
                                <notes><xsl:value-of select="fmp:c_notes"/></notes>
                                <dc_extent><xsl:value-of select="fmp:c_extent"/></dc_extent>
                            </DesciptiveGroup>       
                            <AdministrativeGroup>
                                <FormationPeriod>
                                    <xsl:value-of select="fmp:c_FormationPeriod"/>
                                </FormationPeriod>
                                <Owner><xsl:value-of select="fmp:c_institution"/></Owner>
                                <PhysicalLocation><xsl:value-of select="fmp:c_location"/></PhysicalLocation>
                                <dc_format><xsl:value-of select="fmp:c_format"/></dc_format>
                                <dc_medium><xsl:value-of select="fmp:c_physical_medium"/></dc_medium>
                                <PhysicalLocation><xsl:value-of select="fmp:c_physical_location_website"/></PhysicalLocation>
                            </AdministrativeGroup>
                            <KeywordsGroup>
                                <Collector><xsl:value-of select="fmp:c_collector"/></Collector>       
                            </KeywordsGroup>
                        </Collection>                            
                    </Collections>                        
                </RecordSet>                    
            </xsl:for-each>
               
        </xsl:template>
        <xsl:template name="keywordsgroup">
           
            
        </xsl:template>
        <xsl:template name="administrative">
            
        </xsl:template>
        <xsl:template name="descriptive">
                        
                        
        </xsl:template>
        
        
        <xsl:template name="about">
                   
            
            
        </xsl:template>
       
       
</xsl:stylesheet>