<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns="urn:isbn:1-931666-33-4"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:eac="http://www.filemaker.com/fmpdsoresult" 
    xmlns:date="http://exslt.org/dates-and-times"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    exclude-result-prefixes="eac"
    extension-element-prefixes="date">
    <xsl:output method="xml" omit-xml-declaration="yes" indent="yes" encoding="UTF-8"/>
    <xsl:include href="Utils.xsl"/>
    <!-- EAC-CPF: Generates a EAC-CPF document for each row in the FileMaker Exported File -->
    <xsl:template name="eaccpf">
        <xsl:param name="id"/>
        <xsl:param name="maintenance_status"/>
        <xsl:param name="publication_status"/>
        <xsl:param name="agency"/>
        <xsl:param name="created"/>
        <xsl:param name="edited"/>
        <xsl:param name="harvested"/>
        <xsl:param name="sources"/>
        <xsl:param name="type"/>
        <xsl:param name="primary_name"/>
        <xsl:param name="abbrivated_name"/>
        <xsl:param name="alternative_names"/>
        <xsl:param name="start_date"/>
        <xsl:param name="end_date"/>
        <xsl:param name="exist_date_description"/>
        <xsl:param name="description"/>
        <xsl:param name="address_line1"/>
        <xsl:param name="address_line2"/>
        <xsl:param name="city"/>
        <xsl:param name="state"/>
        <xsl:param name="postal_code"/>
        <xsl:param name="country"/>
        <xsl:param name="url"/>
        <xsl:param name="phone"/>
        <xsl:param name="email_address"/>
        <xsl:param name="occupations"/>
        <xsl:param name="languages"/>
        <xsl:param name="language_codes"/>
        <xsl:param name="scripts"/>
        <xsl:param name="script_codes"/>
        <eac-cpf xmlns="urn:isbn:1-931666-33-4"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:xlink="http://www.w3.org/1999/xlink"
            xsi:schemaLocation="urn:isbn:1-931666-33-4 http://eac.staatsbibliothek-berlin.de/schema/cpf.xsd">
            <xsl:call-template name="control">
                <xsl:with-param name="id" select="$id"/>
                <xsl:with-param name="maintenance_status" select="$maintenance_status"/>
                <xsl:with-param name="publication_status" select="$publication_status"/>
                <xsl:with-param name="agency" select="$agency"/>
                <xsl:with-param name="created" select="$created"/>
                <xsl:with-param name="edited" select="$edited"/>
                <xsl:with-param name="harvested" select="$harvested"/>
                <xsl:with-param name="sources" select="$sources"/>
            </xsl:call-template>
            <xsl:call-template name="cpfDescription">
                <xsl:with-param name="id" select="$id"/>
                <xsl:with-param name="type" select="$type"/>
                <xsl:with-param name="primary_name" select="$primary_name"/>
                <xsl:with-param name="abbrivated_name" select="$abbrivated_name"/>
                <xsl:with-param name="alternative_names" select="$alternative_names"/>
                <xsl:with-param name="start_date" select="$start_date"/>
                <xsl:with-param name="end_date" select="$end_date"/>
                <xsl:with-param name="exist_date_description" select="$exist_date_description"/>
                <xsl:with-param name="description" select="$description"/>
                <xsl:with-param name="address_line1" select="$address_line1"/>
                <xsl:with-param name="address_line2" select="$address_line2"/>
                <xsl:with-param name="city" select="$city"/>
                <xsl:with-param name="state" select="$state"/>
                <xsl:with-param name="postal_code" select="$postal_code"/>
                <xsl:with-param name="country" select="$country"/>
                <xsl:with-param name="url" select="$url"/>
                <xsl:with-param name="phone" select="$phone"/>
                <xsl:with-param name="email_address" select="$email_address"/>
                <xsl:with-param name="occupations" select="$occupations"/>
                <xsl:with-param name="languages" select="$languages"/>
                <xsl:with-param name="language_codes" select="$language_codes"/>
                <xsl:with-param name="scripts" select="$scripts"/>
                <xsl:with-param name="script_codes" select="$script_codes"/>
            </xsl:call-template>
        </eac-cpf>
    </xsl:template>
    <!-- control -->
    <xsl:template name="control">
        <xsl:param name="id"/>
        <xsl:param name="maintenance_status"/>
        <xsl:param name="publication_status"/>
        <xsl:param name="agency"/>
        <xsl:param name="created"/>
        <xsl:param name="edited"/>
        <xsl:param name="harvested"/>
        <xsl:param name="sources"/>
        <control>
            <xsl:call-template name="recordID">
                <xsl:with-param name="id" select="$id"/>
            </xsl:call-template>
            <xsl:call-template name="maintenanceStatus">
                <xsl:with-param name="value" select="$maintenance_status"/>
            </xsl:call-template>
            <xsl:call-template name="publicationStatus">
                <xsl:with-param name="value" select="$publication_status"/>
            </xsl:call-template>
            <xsl:call-template name="maintenanceAgency">
                <xsl:with-param name="agency" select="$agency"/>
            </xsl:call-template>
            <xsl:call-template name="maintenanceHistory">
                <xsl:with-param name="agency" select="$agency"/>
                <xsl:with-param name="created" select="$created"/>
                <xsl:with-param name="edited" select="$edited"/>
                <xsl:with-param name="harvested" select="$harvested"/>
            </xsl:call-template>
            <xsl:call-template name="sources">
                <xsl:with-param name="sources" select="$sources"/>
            </xsl:call-template>
        </control>
    </xsl:template>
    <!-- Record ID -->
    <xsl:template name="recordID">
        <xsl:param name="id"/>
        <recordId>
            <xsl:value-of select="$id"/>
        </recordId>
    </xsl:template>
    <!-- Maintenance Agency -->
    <xsl:template name="maintenanceAgency">
        <xsl:param name="agency"/>
        <maintenanceAgency>
            <agencyName>
                <xsl:value-of select="$agency"/>
            </agencyName>
        </maintenanceAgency>
    </xsl:template>
    <!-- Maintenance Status: Defaults to 'new' -->
    <xsl:template name="maintenanceStatus">
        <xsl:param name="value"/>
        <maintenanceStatus>
            <xsl:choose>
                <xsl:when test="$value = 'revised'">
                    <xsl:text>revised</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'deleted'">
                    <xsl:text>deleted</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'new'">
                    <xsl:text>new</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'deletedSplit'">
                    <xsl:text>deletedSplit</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'deletedReplaced'">
                    <xsl:text>deletedReplaced</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'cancelled'">
                    <xsl:text>cancelled</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'derived'">
                    <xsl:text>derived</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text>new</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
        </maintenanceStatus>
    </xsl:template>
    <!-- Publication Status: Defaults to 'inProgress' -->
    <xsl:template name="publicationStatus">
        <xsl:param name="value"/>
        <publicationStatus>
            <xsl:choose>
                <xsl:when test="$value = 'inProcess'">
                    <xsl:text>inProcess</xsl:text>
                </xsl:when>
                <xsl:when test="$value = 'approved'">
                    <xsl:text>approved</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text>inProcess</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
        </publicationStatus>
    </xsl:template>
    <!-- Maintenance History -->
    <xsl:template name="maintenanceHistory">
        <xsl:param name="agency"/>
        <xsl:param name="created"/>
        <xsl:param name="edited"/>
        <xsl:param name="harvested"/>
        <maintenanceHistory>
            <xsl:choose>
                <xsl:when test="normalize-space($created)">
                    <xsl:call-template name="maintenanceEvent">
                        <xsl:with-param name="type">created</xsl:with-param>
                        <xsl:with-param name="date" select="$created"/>
                        <xsl:with-param name="agency" select="$agency"/>
                        <xsl:with-param name="description">Record Created</xsl:with-param>
                    </xsl:call-template>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:variable name="date" select="date:date()"/>
                    <xsl:variable name="year" select="substring($date, '1', '4')"/>
                    <xsl:variable name="month" select="substring($date, '6', '2')"/>
                    <xsl:variable name="day" select="substring($date, '9', '2')"/>
                    <xsl:variable name="formatted" select="concat($month, '/', $day, '/', $year)"/>
                    <xsl:call-template name="maintenanceEvent">
                        <xsl:with-param name="type">created</xsl:with-param>
                        <xsl:with-param name="date" select="$formatted"/>
                        <xsl:with-param name="agency" select="$agency"/>
                        <xsl:with-param name="description">No Record Create Date Given. This is the time approximately at which this file was ingested</xsl:with-param>
                    </xsl:call-template>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:call-template name="maintenanceEvent">
                <xsl:with-param name="type">revised</xsl:with-param>
                <xsl:with-param name="date" select="$edited"/>
                <xsl:with-param name="agency" select="$agency"/>
                <xsl:with-param name="description">Record Edited</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="maintenanceEvent">
                <xsl:with-param name="type">derived</xsl:with-param>
                <xsl:with-param name="date" select="$harvested"/>
                <xsl:with-param name="agency" select="$agency"/>
                <xsl:with-param name="description">Record Harvested</xsl:with-param>
            </xsl:call-template>
        </maintenanceHistory>
    </xsl:template>
    <!-- Maintenance Event -->
    <xsl:template name="maintenanceEvent">
        <xsl:param name="type"/>
        <xsl:param name="date"/>
        <xsl:param name="agency"/>
        <xsl:param name="description"/>
        <xsl:param name="normalized_date">
            <xsl:call-template name="normalizeDate">
                <xsl:with-param name="date" select="$date"/>
            </xsl:call-template>
        </xsl:param>
        <xsl:if test="normalize-space($date)">
            <maintenanceEvent>
                <eventType>
                    <xsl:value-of select="$type"/>
                </eventType>
                <eventDateTime>
                    <xsl:value-of select="$normalized_date"/>
                </eventDateTime>
                <!-- Assuming all agents are human as the data does not seem to specify -->
                <agentType>human</agentType>
                <agent>
                    <xsl:value-of select="$agency"/>
                </agent>
                <eventDescription>
                    <xsl:value-of select="$description"/>
                </eventDescription>
            </maintenanceEvent>
        </xsl:if>
    </xsl:template>
    <!-- Sources -->
    <xsl:template name="sources">
        <xsl:param name="sources"/>
        <xsl:if test="$sources">
            <sources>
                <xsl:for-each select="$sources">
                    <source>
                        <sourceEntry>
                            <xsl:value-of select="normalize-space(text())"/>
                        </sourceEntry>
                    </source>
                </xsl:for-each>
            </sources>
        </xsl:if>
    </xsl:template>
    <!-- cpfDescription: Describes a Single Entity -->
    <xsl:template name="cpfDescription">
        <xsl:param name="id"/>
        <xsl:param name="type"/>
        <xsl:param name="primary_name"/>
        <xsl:param name="abbrivated_name"/>
        <xsl:param name="alternative_names"/>
        <xsl:param name="start_date"/>
        <xsl:param name="end_date"/>
        <xsl:param name="exist_date_description"/>
        <xsl:param name="description"/>
        <xsl:param name="address_line1"/>
        <xsl:param name="address_line2"/>
        <xsl:param name="city"/>
        <xsl:param name="state"/>
        <xsl:param name="postal_code"/>
        <xsl:param name="country"/>
        <xsl:param name="url"/>
        <xsl:param name="phone"/>
        <xsl:param name="email_address"/>
        <xsl:param name="occupations"/>
        <xsl:param name="languages"/>
        <xsl:param name="language_codes"/>
        <xsl:param name="scripts"/>
        <xsl:param name="script_codes"/>
        <cpfDescription>
            <xsl:call-template name="identity">
                <xsl:with-param name="id" select="$id"/>
                <xsl:with-param name="type" select="$type"/>
                <xsl:with-param name="primary_name" select="$primary_name"/>
                <xsl:with-param name="abbrivated_name" select="$abbrivated_name"/>
                <xsl:with-param name="alternative_names" select="$alternative_names"/>
            </xsl:call-template>
            <xsl:call-template name="description">
                <xsl:with-param name="start_date" select="$start_date"/>
                <xsl:with-param name="end_date" select="$end_date"/>
                <xsl:with-param name="exist_date_description" select="$exist_date_description"/>
                <xsl:with-param name="description" select="$description"/>
                <xsl:with-param name="address_line1" select="$address_line1"/>
                <xsl:with-param name="address_line2" select="$address_line2"/>
                <xsl:with-param name="city" select="$city"/>
                <xsl:with-param name="state" select="$state"/>
                <xsl:with-param name="postal_code" select="$postal_code"/>
                <xsl:with-param name="country" select="$country"/>
                <xsl:with-param name="url" select="$url"/>
                <xsl:with-param name="phone" select="$phone"/>
                <xsl:with-param name="email_address" select="$email_address"/>
                <xsl:with-param name="occupations" select="$occupations"/>
                <xsl:with-param name="languages" select="$languages"/>
                <xsl:with-param name="language_codes" select="$language_codes"/>
                <xsl:with-param name="scripts" select="$scripts"/>
                <xsl:with-param name="script_codes" select="$script_codes"/>
            </xsl:call-template>
            <xsl:call-template name="relations">
                <xsl:with-param name="a"/>
            </xsl:call-template>
        </cpfDescription>
    </xsl:template>
    <!-- Identity -->
    <xsl:template name="identity">
        <xsl:param name="id"/>
        <xsl:param name="type"/>
        <xsl:param name="primary_name"/>
        <xsl:param name="abbrivated_name"/>
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
            <xsl:call-template name="abbrivatedName">
                <xsl:with-param name="abbrivated_name" select="$abbrivated_name"/>
            </xsl:call-template>
            <xsl:call-template name="alternativeNames">
                <xsl:with-param name="alternative_names" select="$alternative_names"/>
            </xsl:call-template>
        </identity>
    </xsl:template>
    <!-- Entity Id -->
    <xsl:template name="entityId">
        <xsl:param name="id"/>
        <entityId>
            <xsl:value-of select="$id"/>
        </entityId>
    </xsl:template>
    <!-- Entity Type -->
    <xsl:template name="entityType">
        <xsl:param name="type"/>
        <entityType>
            <xsl:value-of select="$type"/>
        </entityType>
    </xsl:template>
    <!-- Primary Name -->
    <xsl:template name="primaryName">
        <xsl:param name="primary_name"/>
        <xsl:call-template name="nameEntry">
            <xsl:with-param name="type">primary</xsl:with-param>
            <xsl:with-param name="value" select="$primary_name"/>
        </xsl:call-template>
    </xsl:template>
    <!-- Abbrivated Name -->
    <xsl:template name="abbrivatedName">
        <xsl:param name="abbrivated_name"/>
        <xsl:call-template name="nameEntry">
            <xsl:with-param name="type">abbreviation</xsl:with-param>
            <xsl:with-param name="value" select="$abbrivated_name"/>
        </xsl:call-template>
    </xsl:template>
    <!-- Abbrivated Name -->
    <xsl:template name="alternativeNames">
        <xsl:param name="alternative_names"/>
        <xsl:if test="$alternative_names">
           <xsl:for-each select="$alternative_names">
               <xsl:call-template name="nameEntry">
                   <xsl:with-param name="type">alt</xsl:with-param>
                   <xsl:with-param name="value" select="text()"/>
               </xsl:call-template>
           </xsl:for-each>
        </xsl:if>
    </xsl:template>
    <!-- Name Entry -->
    <xsl:template name="nameEntry">
        <xsl:param name="type"/>
        <xsl:param name="value"/>
        <xsl:if test="$value">
            <nameEntry>
                <xsl:attribute name="localType">
                    <xsl:value-of select="$type"/>
                </xsl:attribute>
                <part>
                    <xsl:value-of select="$value"/>
                </part>
            </nameEntry>
        </xsl:if>
    </xsl:template>
    <!-- Description -->
    <xsl:template name="description">
        <xsl:param name="start_date"/>
        <xsl:param name="end_date"/>
        <xsl:param name="exist_date_description"/>
        <xsl:param name="description"/>
        <xsl:param name="address_line1"/>
        <xsl:param name="address_line2"/>
        <xsl:param name="city"/>
        <xsl:param name="state"/>
        <xsl:param name="postal_code"/>
        <xsl:param name="country"/>
        <xsl:param name="url"/>
        <xsl:param name="phone"/>
        <xsl:param name="email_address"/>
        <xsl:param name="occupations"/>
        <xsl:param name="languages"/>
        <xsl:param name="language_codes"/>
        <xsl:param name="scripts"/>
        <xsl:param name="script_codes"/>
        <description>
            <xsl:call-template name="existDates">
                <xsl:with-param name="start_date" select="$start_date"/>
                <xsl:with-param name="end_date" select="$end_date"/>
                <xsl:with-param name="exist_date_description" select="$exist_date_description"/>
            </xsl:call-template>
            <xsl:call-template name="place">
                <xsl:with-param name="address_line1" select="$address_line1"/>
                <xsl:with-param name="address_line2" select="$address_line2"/>
                <xsl:with-param name="city" select="$city"/>
                <xsl:with-param name="state" select="$state"/>
                <xsl:with-param name="postal_code" select="$postal_code"/>
                <xsl:with-param name="country" select="$country"/>
                <xsl:with-param name="url" select="$url"/>
                <xsl:with-param name="phone" select="$phone"/>
                <xsl:with-param name="email_address" select="$email_address"/>
            </xsl:call-template>
            <xsl:call-template name="occupation">
                <xsl:with-param name="occupations" select="$occupations"/>
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
    <!-- Exist Dates -->
    <xsl:template name="existDates">
        <xsl:param name="start_date"/>
        <xsl:param name="end_date"/>
        <xsl:param name="exist_date_description"/>
        <existDates>
            <dateRange>
                <fromDate>
                    <xsl:value-of select="$start_date"/>
                </fromDate>
                <toDate>
                    <xsl:value-of select="$end_date"/>
                </toDate>
            </dateRange>
            <xsl:if test="normalize-space($exist_date_description)">
                <xsl:call-template name="descriptiveNote">
                    <xsl:with-param name="value" select="$exist_date_description"/>
                </xsl:call-template>
            </xsl:if>
        </existDates>
    </xsl:template>
    <!-- Descriptive Note -->
    <xsl:template name="descriptiveNote">
        <xsl:param name="value"/>
        <descriptiveNote>
            <p>
                <xsl:value-of select="normalize-space($value)"/>
            </p>
        </descriptiveNote>
    </xsl:template>
    <!-- Place -->
    <xsl:template name="place">
        <xsl:param name="address_line1"/>
        <xsl:param name="address_line2"/>
        <xsl:param name="city"/>
        <xsl:param name="state"/>
        <xsl:param name="postal_code"/>
        <xsl:param name="country"/>
        <xsl:param name="url"/>
        <xsl:param name="phone"/>
        <xsl:param name="email_address"/>
        <xsl:if test="normalize-space($address_line1 | $address_line2 | $city | $state | $postal_code | $country | $url | $email_address)">
           <place>
               <xsl:call-template name="address">
                   <xsl:with-param name="address_line1" select="$address_line1"/>
                   <xsl:with-param name="address_line2" select="$address_line2"/>
                   <xsl:with-param name="city" select="$city"/>
                   <xsl:with-param name="state" select="$state"/>
                   <xsl:with-param name="postal_code" select="$postal_code"/>
                   <xsl:with-param name="country" select="$country"/>
                   <xsl:with-param name="url" select="$url"/>
                   <xsl:with-param name="phone" select="$phone"/>
                   <xsl:with-param name="email_address" select="$email_address"/>
               </xsl:call-template>
           </place>
        </xsl:if>
    </xsl:template>
    <!-- Address -->
    <xsl:template name="address">
        <xsl:param name="address_line1"/>
        <xsl:param name="address_line2"/>
        <xsl:param name="city"/>
        <xsl:param name="state"/>
        <xsl:param name="postal_code"/>
        <xsl:param name="country"/>
        <xsl:param name="url"/>
        <xsl:param name="phone"/>
        <xsl:param name="email_address"/>
        <address>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">line1</xsl:with-param>
                <xsl:with-param name="value" select="$address_line1"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">line2</xsl:with-param>
                <xsl:with-param name="value" select="$address_line2"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">city</xsl:with-param>
                <xsl:with-param name="value" select="$city"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">state</xsl:with-param>
                <xsl:with-param name="value" select="$state"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">postalcode</xsl:with-param>
                <xsl:with-param name="value" select="$postal_code"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">country</xsl:with-param>
                <xsl:with-param name="value" select="$country"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">url</xsl:with-param>
                <xsl:with-param name="value" select="$url"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">phone</xsl:with-param>
                <xsl:with-param name="value" select="$phone"/>
            </xsl:call-template>
            <xsl:call-template name="addressLine">
                <xsl:with-param name="type">email</xsl:with-param>
                <xsl:with-param name="value" select="$email_address"/>
            </xsl:call-template>                
        </address>
    </xsl:template>
    <!-- Address Line -->
    <xsl:template name="addressLine">
        <xsl:param name="type"/>
        <xsl:param name="value"/>
        <xsl:if test="$value/text()">
            <addressLine localType="{$type}">
                <xsl:value-of select="$value"/>
            </addressLine>
        </xsl:if>
    </xsl:template>
    <!-- localDescriptions -->
    <xsl:template name="localDescriptions">
        <xsl:param name="type"/>
        <xsl:param name="localType" select="$type"/>
        <xsl:param name="terms"/>
        <xsl:if test="$terms">
           <localDescriptions localType="{$type}">
               <xsl:for-each select="$terms">
                   <localDescription localType="{$localType}">
                       <term>
                           <xsl:value-of select="normalize-space(text())"/>
                       </term>
                   </localDescription>
               </xsl:for-each>
           </localDescriptions>
        </xsl:if>
    </xsl:template>
    <!-- Occupations -->
    <xsl:template name="occupation">
        <xsl:param name="occupations"/>
        <xsl:if test="$occupations">
           <xsl:for-each select="$occupations">
               <occupation>
                   <term>
                       <xsl:value-of select="text()"/>
                   </term>
               </occupation>
           </xsl:for-each>
        </xsl:if>
    </xsl:template>
    <!-- Languages Used -->
    <xsl:template name="languagesUsed">
        <xsl:param name="languages"/>
        <xsl:param name="language_codes"/>
        <xsl:param name="scripts"/>
        <xsl:param name="script_codes"/>
        <xsl:if test="$languages[normalize-space(text())] and $language_codes[normalize-space(text())] and $scripts[normalize-space(text())] and $script_codes[normalize-space(text())]">
           <languagesUsed>
               <xsl:for-each select="$languages">
                   <xsl:call-template name="languageUsed">
                       <xsl:with-param name="language" select="."/>
                       <xsl:with-param name="language_code" select="$language_codes[position()]"/>
                       <xsl:with-param name="script" select="$scripts[position()]"/>
                       <xsl:with-param name="script_code" select="$script_codes[position()]"/>
                   </xsl:call-template>
               </xsl:for-each>
           </languagesUsed>
        </xsl:if>
    </xsl:template>
    <!-- Languages Used -->
    <xsl:template name="languageUsed">
        <xsl:param name="language"/>
        <xsl:param name="language_code"/>
        <xsl:param name="script"/>
        <xsl:param name="script_code"/>
        <xsl:if test="normalize-space($language) and normalize-space($language_code) and normalize-space($script) and normalize-space($script_code)">
            <languageUsed>
                <language languageCode="{$language_code}">
                    <xsl:value-of select="$language"/>
                </language>
                <script scriptCode="{$script_code}"><xsl:value-of select="$script"/></script>
            </languageUsed>
        </xsl:if>
    </xsl:template>
    <!-- Biographical or Historical Note: Default empty  -->
    <xsl:template name="biogHist">
        <xsl:param name="description"/>
        <biogHist>
            <p>
                <xsl:value-of select="normalize-space($description)"/>
            </p>
        </biogHist>
    </xsl:template>
    <!-- Relations -->
    <xsl:template name="relations"/>
    <!-- cpfRelation -->
    <xsl:template name="cpfRelation">
        <xsl:param name="type"/>
        <xsl:param name="localType"/>
        <xsl:param name="value"/>
        <xsl:param name="description"/>
        <xsl:variable name="isID">
            <xsl:call-template name="isID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <cpfRelation cpfRelationType="{$type}">
            <xsl:if test="$isID = 'true'">
                <xsl:attribute name="xlink:href">
                    <xsl:value-of select="$value"/>
                </xsl:attribute>
                <xsl:attribute name="xlink:role">
                    <xsl:call-template name="getfedoraAuthorityURI">
                        <xsl:with-param name="value" select="$value"/>
                    </xsl:call-template>
                </xsl:attribute>
            </xsl:if>
            <relationEntry localType="{$localType}">
                <xsl:value-of select="normalize-space($value)"/>
            </relationEntry>
            <xsl:if test="$description">
                <xsl:call-template name="descriptiveNote">
                    <xsl:with-param name="value" select="$description"/>
                </xsl:call-template>
            </xsl:if>
        </cpfRelation>
    </xsl:template>
    <!-- Resource Relation -->
    <xsl:template name="resourceRelation">
        <xsl:param name="type"/>
        <xsl:param name="localType"/>
        <xsl:param name="value"/>
        <xsl:variable name="isID">
            <xsl:call-template name="isID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <resourceRelation resourceRelationType="{$type}">
            <xsl:if test="$isID = 'true'">
                <xsl:attribute name="xlink:href">
                    <xsl:value-of select="$value"/>
                </xsl:attribute>
                <xsl:attribute name="xlink:role">
                    <xsl:call-template name="getfedoraAuthorityURI">
                        <xsl:with-param name="value" select="$value"/>
                    </xsl:call-template>
                </xsl:attribute>
            </xsl:if>
            <relationEntry localType="{$localType}">
                <xsl:value-of select="normalize-space($value)"/>
            </relationEntry>
        </resourceRelation>
    </xsl:template>
</xsl:stylesheet>
