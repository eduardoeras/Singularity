package reader;

import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;

import java.io.IOException;

public class Reader {

    //Attributes

    //Constructor

    //Public Methods
    public ParseTree read (String fileName) throws IOException {
        CharStream charStream = CharStreams.fromFileName(fileName);
        CPP14Lexer cpp14Lexer = new CPP14Lexer(charStream);
        CommonTokenStream commonTokenStream = new CommonTokenStream(cpp14Lexer);
        CPP14Parser cpp14Parser = new CPP14Parser(commonTokenStream);

        ParseTree parseTree = cpp14Parser.translationunit();
        return parseTree;
    }

}
