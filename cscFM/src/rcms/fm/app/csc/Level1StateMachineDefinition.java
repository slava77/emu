package rcms.fm.app.csc;

import rcms.fm.fw.user.UserStateMachineDefinition;

import rcms.statemachine.definition.*;

import rcms.fm.fw.parameter.*;
import rcms.fm.fw.parameter.type.*;

import java.lang.reflect.Field;

public class Level1StateMachineDefinition extends UserStateMachineDefinition {

	public Level1StateMachineDefinition()
			throws StateMachineDefinitionException {
		//
		// Declare states
		addStates(Level1States.class);
		setInitialState(Level1States.INITIAL);

		//
		// Declare inputs
		addInputs(Level1Inputs.class);

		//
		// Declare parameters associated with inputs
		// (initialize, configure, start and testTTS)

		ParameterSet<CommandParameter> initializeParameters =
				new ParameterSet<CommandParameter>();

		try {
			initializeParameters.add(new CommandParameter<StringT>(
					Level1Parameters.SID, new StringT("")));
			initializeParameters.add(new CommandParameter<StringT>(
					Level1Parameters.GLOBAL_CONF_KEY, new StringT("")));
		} catch (ParameterException ignored) {}

		Level1Inputs.INITIALIZE.setParameters(initializeParameters);

		ParameterSet<CommandParameter> testTTSParameters =
				new ParameterSet<CommandParameter>();

		try {
			testTTSParameters.add(new CommandParameter<IntegerT>(
					Level1Parameters.TTS_TEST_FED_ID, new IntegerT(-1)));
			testTTSParameters.add(new CommandParameter<StringT>(
					Level1Parameters.TTS_TEST_MODE, new StringT("")));
			testTTSParameters.add(new CommandParameter<StringT>(
					Level1Parameters.TTS_TEST_PATTERN, new StringT("")));
			testTTSParameters.add(new CommandParameter<IntegerT>(
					Level1Parameters.TTS_TEST_SEQUENCE_REPEAT, new IntegerT(-1)));
		} catch (ParameterException ignored) {}

		Level1Inputs.TTS_TEST.setParameters(testTTSParameters);

		// Declare transitions
		addTransition(Level1Inputs.INITIALIZE,
				Level1States.INITIAL, Level1States.INITIALIZING);
		addTransition(Level1Inputs.INITIALIZE_DONE,
				Level1States.INITIALIZING, Level1States.HALTED);
		addTransition(Level1Inputs.TTS_PREPARE,
				Level1States.HALTED, Level1States.TTS_PREPARING);
		addTransition(Level1Inputs.TTS_PREPARE_DONE,
				Level1States.TTS_PREPARING, Level1States.TTS_READY);
		addTransition(Level1Inputs.TTS_TEST,
				Level1States.TTS_READY, Level1States.TTS_TESTING);
		addTransition(Level1Inputs.TTS_TEST_DONE,
				Level1States.TTS_TESTING, Level1States.TTS_READY);
		addTransition(Level1Inputs.HALT,
				Level1States.TTS_READY, Level1States.HALTING);
		addTransition(Level1Inputs.HALT_DONE,
				Level1States.HALTING, Level1States.HALTED);

		addTransition(Level1Inputs.ERROR, State.ANYSTATE, Level1States.ERROR);
	}

	private void addStates(Class<Level1States> klass)
			throws StateMachineDefinitionException {
		Field fields[] = klass.getFields();

		try {
			for (int i = 0; i < fields.length; ++i) {
				if (fields[i].getType().getName().indexOf("State") >= 0) {
					addState((State)fields[i].get(null));
				}
			}
		} catch (Exception e) {
			throw new StateMachineDefinitionException("addStates", e);
		}
	}

	private void addInputs(Class<Level1Inputs> klass)
			throws StateMachineDefinitionException {
		Field fields[] = klass.getFields();

		try {
			for (int i = 0; i < fields.length; ++i) {
				Field field = fields[i];
				if (field.getType().getName().indexOf("Input") >= 0) {
					String name = field.getName();
					if (name.indexOf("_DONE") > 0 || name.equals("ERROR")) {
						((Input)fields[i].get(null)).setVisualizable(false);
					}
					addInput((Input)fields[i].get(null));
				}
			}
		} catch (Exception e) {
			throw new StateMachineDefinitionException("addInputs", e);
		}
	}
}

// End of file
// vim: set sw=4 ts=4:
