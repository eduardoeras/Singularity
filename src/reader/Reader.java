package reader;

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.tree.TerminalNode;

import java.io.IOException;

public class Reader {

    //Attributes
    private int counter;

    //Constructor
    public Reader () {
        counter = 0;
    }

    //Public Methods
    public void read (String fileName) throws IOException {
        CharStream charStream = CharStreams.fromFileName(fileName);
        CPP14Lexer cpp14Lexer = new CPP14Lexer(charStream);
        CommonTokenStream commonTokenStream = new CommonTokenStream(cpp14Lexer);
        CPP14Parser cpp14Parser = new CPP14Parser(commonTokenStream);

        ParseTree parseTree = cpp14Parser.translationunit();
        printTree(parseTree);

        System.out.println(counter);
    }

    //Private Methods
    private void printTree (ParseTree parseTree) {
        if (parseTree instanceof TerminalNode) {
            System.out.println("getText = " + parseTree.getText());
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            printTree(parseTree.getChild(i));
        }
    }

}
