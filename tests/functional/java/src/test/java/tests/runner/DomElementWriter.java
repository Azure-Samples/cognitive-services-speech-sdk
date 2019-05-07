/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

package tests.runner; //barrypitman.junitXmlFormatter;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import org.w3c.dom.Attr;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;

/**
 * NOTE: This class is from Apache Ant 1.7.1
 * Writes a DOM tree to a given Writer.
 * warning: this utility currently does not declare XML Namespaces.
 * <p>Utility class used by {@link org.apache.tools.ant.XmlLogger
 * XmlLogger} and
 * org.apache.tools.ant.taskdefs.optional.junit.XMLJUnitResultFormatter
 * XMLJUnitResultFormatter}.</p>
 */
class DOMElementWriter
{
    private static final int HEX = 16;

    /** prefix for generated prefixes */
    private static final String NS = "ns";

    /** xml declaration is on by default */
    private boolean xmlDeclaration = true;

    /**
     * XML Namespaces are ignored by default.
     */
    private XmlNamespacePolicy namespacePolicy = XmlNamespacePolicy.IGNORE;

    /**
     * Map (URI to prefix) of known namespaces.
     */
    private HashMap<String, String> nsPrefixMap = new HashMap<String, String>();

    /**
     * Number of generated prefix to use next.
     */
    private int nextPrefix = 0;

    /**
     * Map (Element to URI) of namespaces defined on a given element.
     */
    private HashMap<Element, ArrayList<String>> nsURIByElement = new HashMap<Element, ArrayList<String>>();

    /**
     * Whether namespaces should be ignored for elements and attributes.
     *
     * @since Ant 1.7
     */
    static class XmlNamespacePolicy
    {
        private boolean qualifyElements;
        private boolean qualifyAttributes;

        /**
         * Ignores namespaces for elements and attributes, the default.
         */
        public static final XmlNamespacePolicy IGNORE = new XmlNamespacePolicy(false, false);

        /**
         * Ignores namespaces for attributes.
         */
        public static final XmlNamespacePolicy ONLY_QUALIFY_ELEMENTS = new XmlNamespacePolicy(true, false);

        /**
         * Qualifies namespaces for elements and attributes.
         */
        public static final XmlNamespacePolicy QUALIFY_ALL = new XmlNamespacePolicy(true, true);

        /**
         * @param qualifyElements whether to qualify elements
         * @param qualifyAttributes whether to qualify elements
         */
        public XmlNamespacePolicy(boolean qualifyElements, boolean qualifyAttributes)
        {
            this.qualifyElements = qualifyElements;
            this.qualifyAttributes = qualifyAttributes;
        }
    }

    /**
     * Create an element writer.
     * The ?xml? declaration will be included, namespaces ignored.
     */
    DOMElementWriter()
    {
    }

    /**
     * Create an element writer
     * XML namespaces will be ignored.
     * @param xmlDeclaration flag to indicate whether the ?xml? declaration
     * should be included.
     * @since Ant1.7
     */
    DOMElementWriter(boolean xmlDeclaration)
    {
        this(xmlDeclaration, XmlNamespacePolicy.IGNORE);
    }

    /**
     * Create an element writer
     * XML namespaces will be ignored.
     * @param xmlDeclaration flag to indicate whether the ?xml? declaration
     * should be included.
     * @param namespacePolicy the policy to use.
     * @since Ant1.7
     */
    DOMElementWriter(boolean xmlDeclaration, XmlNamespacePolicy namespacePolicy)
    {
        this.xmlDeclaration = xmlDeclaration;
        this.namespacePolicy = namespacePolicy;
    }

    private static String lSep = System.getProperty("line.separator");

    // CheckStyle:VisibilityModifier OFF - bc
    /**
     * Don't try to be too smart but at least recognize the predefined
     * entities.
     */
    String[] knownEntities = { "gt", "amp", "lt", "apos", "quot" };

    // CheckStyle:VisibilityModifier ON

    /**
     * Writes a DOM tree to a stream in UTF8 encoding. Note that
     * it prepends the &lt;?xml version='1.0' encoding='UTF-8'?&gt; if
     * the xmlDeclaration field is true.
     * The indent number is set to 0 and a 2-space indent.
     * @param root the root element of the DOM tree.
     * @param out the outputstream to write to.
     * @throws IOException if an error happens while writing to the stream.
     */
    void write(Element root, OutputStream out) throws IOException
    {
        Writer wri = new OutputStreamWriter(out, "UTF8");
        writeXMLDeclaration(wri);
        write(root, wri, 0, "  ");
        wri.flush();
    }

    /**
     * Writes the XML declaration if xmlDeclaration is true.
     * @param wri the writer to write to.
     * @throws IOException if there is an error.
     * @since Ant 1.7.0
     */
    void writeXMLDeclaration(Writer wri) throws IOException
    {
        if (xmlDeclaration)
        {
            wri.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        }
    }

    /**
     * Writes a DOM tree to a stream.
     *
     * @param element the Root DOM element of the tree
     * @param out where to send the output
     * @param indent number of
     * @param indentWith string that should be used to indent the
     * corresponding tag.
     * @throws IOException if an error happens while writing to the stream.
     */
    void write(Element element, Writer out, int indent, String indentWith) throws IOException
    {

        // Write child elements and text
        NodeList children = element.getChildNodes();
        boolean hasChildren = (children.getLength() > 0);
        boolean hasChildElements = false;
        openElement(element, out, indent, indentWith, hasChildren);

        if (hasChildren)
        {
            for (int i = 0; i < children.getLength(); i++)
            {
                Node child = children.item(i);

                switch (child.getNodeType())
                {

                    case Node.ELEMENT_NODE:
                        hasChildElements = true;
                        if (i == 0)
                        {
                            out.write(lSep);
                        }
                        write((Element) child, out, indent + 1, indentWith);
                        break;

                    case Node.TEXT_NODE:
                        out.write(encode(child.getNodeValue()));
                        break;

                    case Node.COMMENT_NODE:
                        out.write("<!--");
                        out.write(encode(child.getNodeValue()));
                        out.write("-->");
                        break;

                    case Node.CDATA_SECTION_NODE:
                        out.write("<![CDATA[");
                        out.write(encodedata(((Text) child).getData()));
                        out.write("]]>");
                        break;

                    case Node.ENTITY_REFERENCE_NODE:
                        out.write('&');
                        out.write(child.getNodeName());
                        out.write(';');
                        break;

                    case Node.PROCESSING_INSTRUCTION_NODE:
                        out.write("<?");
                        out.write(child.getNodeName());
                        String data = child.getNodeValue();
                        if (data != null && data.length() > 0)
                        {
                            out.write(' ');
                            out.write(data);
                        }
                        out.write("?>");
                        break;
                    default:
                        // Do nothing
                }
            }
            closeElement(element, out, indent, indentWith, hasChildElements);
        }
    }

    /**
     * Writes the opening tag - including all attributes -
     * corresponding to a DOM element.
     *
     * @param element the DOM element to write
     * @param out where to send the output
     * @param indent number of
     * @param indentWith string that should be used to indent the
     * corresponding tag.
     * @throws IOException if an error happens while writing to the stream.
     */
    void openElement(Element element, Writer out, int indent, String indentWith) throws IOException
    {
        openElement(element, out, indent, indentWith, true);
    }

    /**
     * Writes the opening tag - including all attributes -
     * corresponding to a DOM element.
     *
     * @param element the DOM element to write
     * @param out where to send the output
     * @param indent number of
     * @param indentWith string that should be used to indent the
     * corresponding tag.
     * @param hasChildren whether this element has children.
     * @throws IOException if an error happens while writing to the stream.
     * @since Ant 1.7
     */
    void openElement(Element element, Writer out, int indent, String indentWith, boolean hasChildren) throws IOException
    {
        // Write indent characters
        for (int i = 0; i < indent; i++)
        {
            out.write(indentWith);
        }

        // Write element
        out.write("<");
        if (namespacePolicy.qualifyElements)
        {
            String uri = getNamespaceURI(element);
            String prefix = (String) nsPrefixMap.get(uri);
            if (prefix == null)
            {
                if (nsPrefixMap.isEmpty())
                {
                    // steal default namespace
                    prefix = "";
                }
                else
                {
                    prefix = NS + (nextPrefix++);
                }
                nsPrefixMap.put(uri, prefix);
                addNSDefinition(element, uri);
            }
            if (!"".equals(prefix))
            {
                out.write(prefix);
                out.write(":");
            }
        }
        out.write(element.getTagName());

        // Write attributes
        NamedNodeMap attrs = element.getAttributes();
        for (int i = 0; i < attrs.getLength(); i++)
        {
            Attr attr = (Attr) attrs.item(i);
            out.write(" ");
            if (namespacePolicy.qualifyAttributes)
            {
                String uri = getNamespaceURI(attr);
                String prefix = (String) nsPrefixMap.get(uri);
                if (prefix == null)
                {
                    prefix = NS + (nextPrefix++);
                    nsPrefixMap.put(uri, prefix);
                    addNSDefinition(element, uri);
                }
                out.write(prefix);
                out.write(":");
            }
            out.write(attr.getName());
            out.write("=\"");
            out.write(encode(attr.getValue()));
            out.write("\"");
        }

        // write namespace declarations
        ArrayList<?> al = (ArrayList<?>) nsURIByElement.get(element);
        if (al != null)
        {
            Iterator<?> iter = al.iterator();
            while (iter.hasNext())
            {
                String uri = (String) iter.next();
                String prefix = (String) nsPrefixMap.get(uri);
                out.write(" xmlns");
                if (!"".equals(prefix))
                {
                    out.write(":");
                    out.write(prefix);
                }
                out.write("=\"");
                out.write(uri);
                out.write("\"");
            }
        }

        if (hasChildren)
        {
            out.write(">");
        }
        else
        {
            removeNSDefinitions(element);
            out.write(" />");
            out.write(lSep);
            out.flush();
        }
    }

    /**
     * Writes a DOM tree to a stream.
     *
     * @param element the Root DOM element of the tree
     * @param out where to send the output
     * @param indent number of
     * @param indentWith string that should be used to indent the
     * corresponding tag.
     * @param hasChildren if true indent.
     * @throws IOException if an error happens while writing to the stream.
     */
    void closeElement(Element element, Writer out, int indent, String indentWith, boolean hasChildren) throws IOException
    {
        // If we had child elements, we need to indent before we close
        // the element, otherwise we're on the same line and don't need
        // to indent
        if (hasChildren)
        {
            for (int i = 0; i < indent; i++)
            {
                out.write(indentWith);
            }
        }

        // Write element close
        out.write("</");
        if (namespacePolicy.qualifyElements)
        {
            String uri = getNamespaceURI(element);
            String prefix = (String) nsPrefixMap.get(uri);
            if (prefix != null && !"".equals(prefix))
            {
                out.write(prefix);
                out.write(":");
            }
            removeNSDefinitions(element);
        }
        out.write(element.getTagName());
        out.write(">");
        out.write(lSep);
        out.flush();
    }

    /**
     * Escape &lt;, &gt; &amp; &apos;, &quot; as their entities and
     * drop characters that are illegal in XML documents.
     * @param value the string to encode.
     * @return the encoded string.
     */
    String encode(String value)
    {
        StringBuffer sb = new StringBuffer();
        int len = value.length();
        for (int i = 0; i < len; i++)
        {
            char c = value.charAt(i);
            switch (c)
            {
                case '<':
                    sb.append("&lt;");
                    break;
                case '>':
                    sb.append("&gt;");
                    break;
                case '\'':
                    sb.append("&apos;");
                    break;
                case '\"':
                    sb.append("&quot;");
                    break;
                case '&':
                    int nextSemi = value.indexOf(";", i);
                    if (nextSemi < 0 || !isReference(value.substring(i, nextSemi + 1)))
                    {
                        sb.append("&amp;");
                    }
                    else
                    {
                        sb.append('&');
                    }
                    break;
                default:
                    if (isLegalCharacter(c))
                    {
                        sb.append(c);
                    }
                    break;
            }
        }
        return sb.substring(0);
    }

    /**
     * Drop characters that are illegal in XML documents.
     *
     * <p>Also ensure that we are not including an <code>]]&gt;</code>
     * marker by replacing that sequence with
     * <code>&amp;#x5d;&amp;#x5d;&amp;gt;</code>.</p>
     *
     * <p>See XML 1.0 2.2 <a
     * href="http://www.w3.org/TR/1998/REC-xml-19980210#charsets">
     * http://www.w3.org/TR/1998/REC-xml-19980210#charsets</a> and
     * 2.7 <a
     * href="http://www.w3.org/TR/1998/REC-xml-19980210#sec-cdata-sect">http://www.w3.org/TR/1998/REC-xml-19980210#sec-cdata-sect</a>.</p>
     * @param value the value to be encoded.
     * @return the encoded value.

     */
    String encodedata(final String value)
    {
        StringBuffer sb = new StringBuffer();
        int len = value.length();
        for (int i = 0; i < len; ++i)
        {
            char c = value.charAt(i);
            if (isLegalCharacter(c))
            {
                sb.append(c);
            }
        }

        String result = sb.substring(0);
        int cdEnd = result.indexOf("]]>");
        while (cdEnd != -1)
        {
            sb.setLength(cdEnd);
            // CheckStyle:MagicNumber OFF
            sb.append("&#x5d;&#x5d;&gt;").append(result.substring(cdEnd + 3));
            // CheckStyle:MagicNumber ON
            result = sb.substring(0);
            cdEnd = result.indexOf("]]>");
        }

        return result;
    }

    /**
     * Is the given argument a character or entity reference?
     * @param ent the value to be checked.
     * @return true if it is an entity.
     */
    boolean isReference(String ent)
    {
        if (!(ent.charAt(0) == '&') || !ent.endsWith(";"))
        {
            return false;
        }

        if (ent.charAt(1) == '#')
        {
            if (ent.charAt(2) == 'x')
            {
                try
                {
                    // CheckStyle:MagicNumber OFF
                    Integer.parseInt(ent.substring(3, ent.length() - 1), HEX);
                    // CheckStyle:MagicNumber ON
                    return true;
                }
                catch (NumberFormatException nfe)
                {
                    return false;
                }
            }
            else
            {
                try
                {
                    Integer.parseInt(ent.substring(2, ent.length() - 1));
                    return true;
                }
                catch (NumberFormatException nfe)
                {
                    return false;
                }
            }
        }

        String name = ent.substring(1, ent.length() - 1);
        for (int i = 0; i < knownEntities.length; i++)
        {
            if (name.equals(knownEntities[i]))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Is the given character allowed inside an XML document?
     *
     * <p>See XML 1.0 2.2 <a
     * href="http://www.w3.org/TR/1998/REC-xml-19980210#charsets">
     * http://www.w3.org/TR/1998/REC-xml-19980210#charsets</a>.</p>
     * @param c the character to test.
     * @return true if the character is allowed.
     * @since 1.10, Ant 1.5
     */
    boolean isLegalCharacter(char c)
    {
        // CheckStyle:MagicNumber OFF
        if (c == 0x9 || c == 0xA || c == 0xD)
        {
            return true;
        }
        else if (c < 0x20)
        {
            return false;
        }
        else if (c <= 0xD7FF)
        {
            return true;
        }
        else if (c < 0xE000)
        {
            return false;
        }
        else if (c <= 0xFFFD)
        {
            return true;
        }
        // CheckStyle:MagicNumber ON
        return false;
    }

    private void removeNSDefinitions(Element element)
    {
        ArrayList<?> al = (ArrayList<?>) nsURIByElement.get(element);
        if (al != null)
        {
            Iterator<?> iter = al.iterator();
            while (iter.hasNext())
            {
                nsPrefixMap.remove(iter.next());
            }
            nsURIByElement.remove(element);
        }
    }

    private void addNSDefinition(Element element, String uri)
    {
        ArrayList<String> al = (ArrayList<String>) nsURIByElement.get(element);
        if (al == null)
        {
            al = new ArrayList<String>();
            nsURIByElement.put(element, al);
        }
        al.add(uri);
    }

    private static String getNamespaceURI(Node n)
    {
        String uri = n.getNamespaceURI();
        if (uri == null)
        {
            // FIXME: Is "No Namespace is Empty Namespace" really OK?
            uri = "";
        }
        return uri;
    }
}