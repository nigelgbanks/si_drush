<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    version="1.0">
    <!-- Normalize Date -->
    <xsl:template name="normalizeDate">
        <xsl:param name="date"/>
        <xsl:variable name="temp" select="translate($date, '/—', '--')"/>
        <xsl:variable name="_month" select="substring-before($temp, '-')"/>
        <xsl:variable name="_day" select="substring-before(substring-after($temp, '-'), '-')"/>
        <xsl:variable name="year" select="substring-after(substring-after($temp, '-'), '-')"/>
        <xsl:variable name="month">
            <xsl:call-template name="normalizeDayOrMonth">
                <xsl:with-param name="num" select="$_month"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="day">
            <xsl:call-template name="normalizeDayOrMonth">
                <xsl:with-param name="num" select="$_day"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="$year and $month and $day">
                <xsl:value-of select="concat($year, '-', $month, '-', $day)"/>    
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$date"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <xsl:template name="normalizeDayOrMonth">
        <xsl:param name="num"/>
        <xsl:if test="string-length($num) = 1">
            <xsl:value-of select="concat('0', $num)"/>
        </xsl:if>
        <xsl:if test="string-length($num) = 2">
            <xsl:value-of select="$num"/>
        </xsl:if>
    </xsl:template>
    <!-- Checks if the given value is an ID -->
    <xsl:template name="isID">
        <xsl:param name="value"/>
        <xsl:variable name="isMODS">
            <xsl:call-template name="isMODSID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isNCD">
            <xsl:call-template name="isNCDID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEAC">
            <xsl:call-template name="isEACID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="$isMODS = 'true' or $isNCD = 'true' or $isEAC = 'true'">
                <xsl:value-of select="true()"/>    
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <!-- Checks to see if the given value is a MODS ID -->
    <xsl:template name="isMODSID">
        <xsl:param name="value"/>
        <xsl:call-template name="_isID">
            <xsl:with-param name="value" select="$value"/>
            <xsl:with-param name="prefix">MODS</xsl:with-param>
        </xsl:call-template>
    </xsl:template>
    <!-- Checks to see if the given value is a MODS ID -->
    <xsl:template name="isNCDID">
        <xsl:param name="value"/>
        <xsl:call-template name="_isID">
            <xsl:with-param name="value" select="$value"/>
            <xsl:with-param name="prefix">NCD</xsl:with-param>
        </xsl:call-template>
    </xsl:template>
    <!-- Checks if the given value is a EAC ID -->
    <xsl:template name="isEACID">
        <xsl:param name="value"/>
        <xsl:call-template name="_isID">
            <xsl:with-param name="value" select="$value"/>
            <xsl:with-param name="prefix">EAC</xsl:with-param>
        </xsl:call-template>
    </xsl:template>
    <!-- Checks if the given value is a EAC Person ID -->
    <xsl:template name="isEACPID">
        <xsl:param name="value"/>
        <xsl:call-template name="_isID">
            <xsl:with-param name="value" select="$value"/>
            <xsl:with-param name="prefix">EACP</xsl:with-param>
        </xsl:call-template>
    </xsl:template>
    <!-- Checks if the given value is a EAC Organization ID -->
    <xsl:template name="isEACOID">
        <xsl:param name="value"/>
        <xsl:call-template name="_isID">
            <xsl:with-param name="value" select="$value"/>
            <xsl:with-param name="prefix">EACO</xsl:with-param>
        </xsl:call-template>
    </xsl:template>
    <!-- Checks if the given value is a EAC Expedition ID -->
    <xsl:template name="isEACEID">
        <xsl:param name="value"/>
        <xsl:call-template name="_isID">
            <xsl:with-param name="value" select="$value"/>
            <xsl:with-param name="prefix">EACE</xsl:with-param>
        </xsl:call-template>
    </xsl:template>
    <!-- Checks if the given value is an ID -->
    <xsl:template name="_isID">
        <xsl:param name="value"/>
        <xsl:param name="prefix"/>
        <xsl:choose>
            <xsl:when test="substring($value, 1, string-length($prefix)) = $prefix">
                <xsl:value-of select="true()"/>    
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <!-- Local Authority -->
    <xsl:template name="getfedoraAuthority">
        <xsl:param name="value"/>
        <xsl:variable name="isMODS">
            <xsl:call-template name="isMODSID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isNCD">
            <xsl:call-template name="isNCDID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEACP">
            <xsl:call-template name="isEACPID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEACO">
            <xsl:call-template name="isEACOID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEACE">
            <xsl:call-template name="isEACEID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="$isMODS = 'true'">
                <xsl:text>si:fieldbooks</xsl:text>
            </xsl:when>
            <xsl:when test="$isNCD = 'true'">
                <xsl:text>si:collections</xsl:text>
            </xsl:when>
            <xsl:when test="$isEACP = 'true'">
                <xsl:text>si:people</xsl:text>
            </xsl:when>
            <xsl:when test="$isEACO = 'true'">
                <xsl:text>si:organizations</xsl:text>
            </xsl:when>
            <xsl:when test="$isEACE = 'true'">
                <xsl:text>si:expeditions</xsl:text>
            </xsl:when>
        </xsl:choose>
    </xsl:template>
    <!-- Local Authority URI -->
    <xsl:template name="getfedoraAuthorityURI">
        <xsl:param name="value"/>
        <xsl:variable name="isMODS">
            <xsl:call-template name="isMODSID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isNCD">
            <xsl:call-template name="isNCDID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEACP">
            <xsl:call-template name="isEACPID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEACO">
            <xsl:call-template name="isEACOID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="isEACE">
            <xsl:call-template name="isEACEID">
                <xsl:with-param name="value" select="$value"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="$isMODS = 'true'">
                <xsl:text>info:islandora/islandora-system:def/smithsonian#modsId</xsl:text>
            </xsl:when>
            <xsl:when test="$isNCD = 'true'">
                <xsl:text>info:islandora/islandora-system:def/smithsonian#ncdId</xsl:text>
            </xsl:when>
            <xsl:when test="$isEACP = 'true'">
                <xsl:text>info:islandora/islandora-system:def/smithsonian#eacpId</xsl:text>
            </xsl:when>
            <xsl:when test="$isEACO = 'true'">
                <xsl:text>info:islandora/islandora-system:def/smithsonian#eacoId</xsl:text>
            </xsl:when>
            <xsl:when test="$isEACE = 'true'">
                <xsl:text>info:islandora/islandora-system:def/smithsonian#eaceId</xsl:text>
            </xsl:when>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>