package extractor;

import extractor.tools.XmlPrinter;
import extractor.transition.TransitionFilter;
import global.structure.Line;
import extractor.state.StateWalker;
import extractor.state.Matrix;
import extractor.transition.TransitionWalker;
import global.structure.State;
import global.structure.Transition;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Extractor {
    //Attributes
    private Matrix matrix;
    private StateWalker stateWalker;
    private TransitionWalker transitionWalker;
    private TransitionFilter transitionFilter;
    private List<State> states;
    private List<Transition> transitions;

    //Constructor
    public Extractor () {
        matrix = new Matrix();
        stateWalker = new StateWalker();
        transitionWalker = new TransitionWalker();
        transitionFilter = new TransitionFilter();
        states = new ArrayList<>();
        transitions = new ArrayList<>();
    }

    //Public Methods
    public void extract (ParseTree parseTree, boolean debug) {
        states.clear();
        transitions.clear();
        List<Line> cppMatrix = new ArrayList<>();
        cppMatrix.add(new Line());

        matrix.matrixGenerator(cppMatrix, parseTree);
        stateWalker.walk(cppMatrix, states);
        transitionWalker.walk(states, transitions);
        transitionFilter.filter(transitions);

        XmlPrinter xmlPrinter = new XmlPrinter();
        xmlPrinter.print(states, transitions, debug);
    }

    public List<State> getStates () {
        return states;
    }

    public List<Transition> getTransitions () {
        return transitions;
    }

}
