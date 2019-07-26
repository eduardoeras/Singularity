package extractor;

import extractor.state.StateWalker;
import extractor.tools.Matrix;
import extractor.transition.TransitionWalker;
import global.structure.Instruction;
import global.structure.Transition;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Extractor {
    //Attributes
    private Matrix matrix;
    private StateWalker stateWalker;
    private TransitionWalker transitionWalker;
    //Constructor
    public Extractor () {
        matrix = new Matrix();
        stateWalker = new StateWalker();
        transitionWalker = new TransitionWalker();
    }

    //Public Methods
    public void extract (ParseTree parseTree) {
        List<Instruction> states = new ArrayList<>();
        List<Transition> transitions = new ArrayList<>();
        List<List<ParseTree>> cppMatrix = new ArrayList<>();
        List<ParseTree> line = new ArrayList<>();
        cppMatrix.add(line);

        matrix.matrixGenerator(cppMatrix, parseTree);
        stateWalker.walk(cppMatrix, states);
        transitionWalker.walk(cppMatrix, transitions);
    }

    //PrivateMethods
    /*private void printMatrix (List<List<ParseTree>> cppMatrix) {
        for (List<ParseTree> line : cppMatrix) {
            for (ParseTree element : line) {
                System.out.print (element.getText() + " ");
            }
            System.out.println();
        }
    }*/

}
