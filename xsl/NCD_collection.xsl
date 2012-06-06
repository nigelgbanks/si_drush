<?xml version="1.0" encoding="utf-8"?>
<!--takes data exported from Smithsonian's filemaker db and transforms to EAC.  This xslt works with the filemaker person records-->
<xsl:stylesheet version="1.0" 
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fmp="http://www.filemaker.com/fmpdsoresult" exclude-result-prefixes="fmp">
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <xsl:include href="Utils.xsl"/>
    <xsl:template match="/">
        <xsl:for-each select="//fmp:ROW">
            <RecordSet xmlns="http://rs.tdwg.org/ncd/0.70"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:xlink="http://www.w3.org/1999/xlink"
                xsi:schemaLocation="http://rs.tdwg.org/ncd/0.70 file:///Users/nigelbanks/Downloads/ncdLite_v1_2-1.xsd">
                <Collections>
                    <Collection id="{fmp:collectionID}">
                        <CollectionId>
                            <xsl:value-of select="fmp:collectionID"/>
                        </CollectionId>
                        <AboutThisRecord>
                            <dc_source>
                                <xsl:value-of select="fmp:c_corporate_affiliation"/>
                            </dc_source>
                            <dc_identifier>
                                <xsl:value-of select="fmp:collectionID"/>
                            </dc_identifier>
                            <dct_created>
                                <xsl:call-template name="normalizeDate">
                                    <xsl:with-param name="date" select="fmp:c_record_created_date"/>
                                </xsl:call-template>
                            </dct_created>
                            <dct_creator>
                                <xsl:value-of select="fmp:c_author"/>
                            </dct_creator>
                            <dct_modified>
                                <xsl:call-template name="normalizeDate">
                                    <xsl:with-param name="date" select="fmp:c_record_edited_date"/>
                                </xsl:call-template>
                            </dct_modified>
                            <xsl:if test="normalize-space(fmp:c_record_harvest_date)">
                                <dct_harvested>
                                    <xsl:call-template name="normalizeDate">
                                        <xsl:with-param name="date" select="fmp:c_record_harvest_date"/>
                                    </xsl:call-template>
                                </dct_harvested>
                            </xsl:if>
                        </AboutThisRecord>
                        <AlternativeIds>
                            <Identifier id="{fmp:c_altID}" source="Smithsonian"/>
                        </AlternativeIds>
                        <DescriptiveGroup>
                            <dc_title>
                                <xsl:value-of select="fmp:c_primary_name"/>
                            </dc_title>
                            <dc_alternative_title>
                                <xsl:value-of select="fmp:c_alt_name"/>
                            </dc_alternative_title>
                            <dc_description>
                                <xsl:value-of select="normalize-space(fmp:c_descr)"/>
                            </dc_description>
                            <dc_extent>
                                <xsl:value-of select="fmp:c_extent"/>
                            </dc_extent>
                            <Notes>
                                <xsl:value-of select="normalize-space(fmp:c_notes)"/>
                            </Notes>
                        </DescriptiveGroup>
                        <AdministrativeGroup>
                            <FormationPeriod>
                                <xsl:value-of select="fmp:c_FormationPeriod"/>
                            </FormationPeriod>
                            <dc_format>
                                <xsl:value-of select="fmp:c_format"/>
                            </dc_format>
                            <dc_medium>
                                <xsl:value-of select="fmp:c_physical_medium"/>
                            </dc_medium>
                            <UsageRestriction>
                                <xsl:value-of select="normalize-space(fmp:c_access_condition)"/>
                            </UsageRestriction>
                            <Contact>
                                <xsl:value-of select="normalize-space(fmp:c_contact)"/>
                            </Contact>
                            <Owner>
                                <xsl:value-of select="fmp:c_institution"/>
                            </Owner>
                            <PhysicalLocation>
                                <xsl:if test="normalize-space(fmp:c_physical_location_website)">
                                    <xsl:attribute name="xlink:href">
                                        <xsl:value-of select="fmp:c_physical_location_website"/>
                                    </xsl:attribute>
                                </xsl:if>
                                <xsl:value-of select="fmp:c_location"/>
                            </PhysicalLocation>
                        </AdministrativeGroup>
                        <KeywordsGroup>
                            <xsl:for-each select="fmp:c_kingdomCoverage/fmp:DATA[normalize-space(text())]">
                                <KingdomCoverage>
                                    <xsl:value-of select="text()"/>
                                </KingdomCoverage>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_taxon_name_coverage/fmp:DATA[normalize-space(text())]">
                                <TaxonCoverage>
                                    <xsl:value-of select="text()"/>
                                </TaxonCoverage>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_common_name/fmp:DATA[normalize-space(text())]">
                                <CommonNameCoverage>
                                    <xsl:value-of select="text()"/>
                                </CommonNameCoverage>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_geospatial_coverage/fmp:DATA[normalize-space(text())]">
                                <GeospatialCoverage>
                                    <xsl:value-of select="text()"/>
                                </GeospatialCoverage>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_living_time_period/fmp:DATA[normalize-space(text())]">
                                <LivingTimePeriod>
                                    <xsl:value-of select="text()"/>
                                </LivingTimePeriod>
                            </xsl:for-each>                            
                            <xsl:for-each select="fmp:c_expedition/fmp:DATA[normalize-space(text())]">
                                <ExpeditionName>
                                    <xsl:value-of select="text()"/>
                                </ExpeditionName>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_collector/fmp:DATA[normalize-space(text())]">
                                <Collector>
                                    <xsl:value-of select="text()"/>
                                </Collector>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_associated_person/fmp:DATA[normalize-space(text())]">
                                <AssociatedAgent>
                                    <xsl:value-of select="text()"/>
                                </AssociatedAgent>
                            </xsl:for-each>
                        </KeywordsGroup>
                        <RelatedMaterialsGroup>
                            <xsl:for-each select="fmp:c_related_collections/fmp:DATA[normalize-space(text())]">
                                <dc_relation>
                                    <xsl:value-of select="normalize-space(text())"/>
                                </dc_relation>
                            </xsl:for-each>
                            <xsl:for-each select="fmp:c_related_materials/fmp:DATA[normalize-space(text())]">
                                <RelatedCollection>
                                    <xsl:value-of select="normalize-space(text())"/>
                                </RelatedCollection>
                            </xsl:for-each>
                        </RelatedMaterialsGroup>
                    </Collection>
                </Collections>
            </RecordSet>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
