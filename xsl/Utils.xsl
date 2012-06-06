<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
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
        <xsl:value-of select="concat($year, '-', $month, '-', $day)"/>
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
</xsl:stylesheet>