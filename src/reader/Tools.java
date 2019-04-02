package reader;

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.TerminalNode;

public class Tools {
    //Attributes
    private int level;
    private int classLevel;
    private boolean visibility;
    private boolean forStructure;
    private boolean newLine;

    //Constructor
    public Tools () {
        level = 0;
        classLevel = -1;
        visibility = false;
        forStructure = false;
        newLine = false;
    }

    //Methods
    public void printEverything (ParseTree parseTree) {
        System.out.println(parseTree.getClass() + " -> " + parseTree.getText());
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printEverything(parseTree.getChild(i));
        }
    }

    public void printTerminalNodes (ParseTree parseTree) {
        if (parseTree instanceof TerminalNode) {
            System.out.println(parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printTerminalNodes(parseTree.getChild(i));
        }
    }

    public void printIndentedCode (ParseTree parseTree) {
        if (parseTree instanceof TerminalNode) {
            switch (parseTree.getText()) {
                case "for":
                    forStructure = true;
                    newLine("for ");
                    break;
                case "class":
                    classLevel = level;
                    newLine("class ");
                    break;
                case "public":
                    visibility = true;
                    newLine("public");
                    break;
                case "private":
                    visibility = true;
                    newLine("private");
                    break;
                case "protected":
                    visibility = true;
                    newLine("protected");
                    break;
                case ":":
                    if (visibility) {
                        visibility = false;
                        System.out.print(":\n");
                        indent();
                    } else {
                        System.out.print(":");
                    }
                    break;
                case "{":
                    System.out.print("\n");
                    indent();
                    System.out.print("{\n");
                    level ++;
                    indent();
                    break;
                case "}":
                    System.out.print("\n");
                    forStructure = false;
                    level --;
                    indent();
                    if (classLevel == level) {
                        System.out.print("}");
                        classLevel = -1;
                    } else {
                        System.out.print("}\n");
                        indent();
                    }
                    break;
                case ";":
                    if (forStructure) {
                        System.out.print("; ");
                    } else {
                        System.out.print(";");
                        newLine = true;
                    }
                    break;
                default:
                    newLine(parseTree.getText());
            }

        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printIndentedCode(parseTree.getChild(i));
        }
    }

    //Private Methods
    private void indent () {
        for (int i = 0; i < level; i++) {
            System.out.print("    ");
        }
    }

    private void newLine (String keyword) {
        if (newLine) {
            newLine = false;
            System.out.print("\n");
            indent();
            System.out.print(keyword + " ");
        } else {
            System.out.print(keyword + " ");
        }
    }
}
