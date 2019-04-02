package reader;

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;

import java.io.IOException;

public class ReaderAntlr {

    //Attributes
    private Tools tools;

    //Constructor
    public ReaderAntlr() {
        tools = new Tools();
    }

    //Public Methods
    public void read (String fileName) throws IOException {
        CharStream charStream = CharStreams.fromFileName(fileName);
        CPP14Lexer cpp14Lexer = new CPP14Lexer(charStream);
        CommonTokenStream commonTokenStream = new CommonTokenStream(cpp14Lexer);
        CPP14Parser cpp14Parser = new CPP14Parser(commonTokenStream);

        ParseTree parseTree = cpp14Parser.translationunit();
        tools.printIndentedCode(parseTree);
    }

}
