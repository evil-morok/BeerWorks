/*
 * Sequencer.h
 *
 *  Created on: 05 сент. 2018 г.
 *      Author: Алексей Рогов
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_



template<class SequenceType, class FunctorType, int LastSequence, SequenceType StartSequence, SequenceType StopSequence>
class Sequencer
{
    public:
        typedef void (FunctorType::*Delegate)();

        Sequencer() : CurrentSequence(StartSequence), NextSequence(StartSequence), _SequenceModified(false)
        {

        }
        void AddSequence(SequenceType Sequence, SequenceType NextSequence, Delegate delegate = (Delegate)0)
        {
            _Sequences[Sequence] = NextSequence;
            _Delegates[Sequence] = delegate;
        }
        SequenceType GetSequence()
        {
            return CurrentSequence;
        }
        void SetSequence(SequenceType Sequence)
        {
            NextSequence = Sequence;
            _SequenceModified = true;
        }
        SequenceType GetNextSequence()
        {
            return _Sequences[CurrentSequence];
        }
        void Next()
        {

            if (_Sequences[CurrentSequence] != StopSequence || _SequenceModified)
            {

                PrevSequence = CurrentSequence;
                CurrentSequence = NextSequence;

                if (!_SequenceModified)
                {
                    NextSequence = _Sequences[CurrentSequence];
                }

                _SequenceModified = false;
            }

            if (_Delegates[CurrentSequence] != (Delegate) 0)
            {
                (((FunctorType*) this)->*_Delegates[CurrentSequence])();
            }

        }
        SequenceType CurrentSequence;
        SequenceType PrevSequence;
        SequenceType NextSequence;
    private:
        static const unsigned long _sequencesCnt = LastSequence + 1;
        SequenceType _Sequences[_sequencesCnt];
        Delegate _Delegates[_sequencesCnt];

        bool _SequenceModified;
};

#endif /* SEQUENCER_H_ */
