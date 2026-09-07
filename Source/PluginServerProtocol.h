#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

// Wire format shared by the main app (client) and AiTrackerPluginServer
// (one child process per loaded plugin). Every message is a MemoryBlock
// whose first byte is a MessageType, followed by a type-specific payload
// written/read with MemoryOutputStream/MemoryInputStream.
namespace PluginServerProtocol
{
    enum class MessageType : juce::uint8
    {
        loadResult = 1,   // child -> parent: bool ok, String name, String error
        processBlock = 2, // parent -> child: int32 numSamples, then MIDI events
        audioResult = 3,  // child -> parent: int32 numChannels, int32 numSamples, then interleaved-by-channel floats
        showEditor = 4,   // parent -> child: no payload
        shutdown = 5      // parent -> child: no payload
    };

    inline MessageType readType(const juce::MemoryBlock& mb)
    {
        return mb.getSize() > 0 ? (MessageType) *static_cast<const juce::uint8*>(mb.getData())
                                 : (MessageType) 0;
    }

    inline juce::MemoryBlock buildLoadResult(bool ok, const juce::String& name, const juce::String& error)
    {
        juce::MemoryOutputStream out;
        out.writeByte((char) MessageType::loadResult);
        out.writeBool(ok);
        out.writeString(name);
        out.writeString(error);
        return out.getMemoryBlock();
    }

    inline void parseLoadResult(const juce::MemoryBlock& mb, bool& ok, juce::String& name, juce::String& error)
    {
        juce::MemoryInputStream in(mb, false);
        in.readByte();
        ok = in.readBool();
        name = in.readString();
        error = in.readString();
    }

    inline juce::MemoryBlock buildProcessBlock(int numSamples, const juce::MidiBuffer& midi)
    {
        juce::MemoryOutputStream out;
        out.writeByte((char) MessageType::processBlock);
        out.writeInt(numSamples);
        out.writeInt(midi.getNumEvents());
        for (const auto metadata : midi)
        {
            out.writeInt(metadata.samplePosition);
            out.writeInt(metadata.numBytes);
            out.write(metadata.data, (size_t) metadata.numBytes);
        }
        return out.getMemoryBlock();
    }

    inline void parseProcessBlock(const juce::MemoryBlock& mb, int& numSamples, juce::MidiBuffer& midi)
    {
        juce::MemoryInputStream in(mb, false);
        in.readByte();
        numSamples = in.readInt();
        const int numEvents = in.readInt();
        midi.clear();
        for (int i = 0; i < numEvents; ++i)
        {
            const int samplePos = in.readInt();
            const int numBytes = in.readInt();
            juce::HeapBlock<juce::uint8> bytes(static_cast<size_t> (numBytes));
            in.read(bytes, numBytes);
            midi.addEvent(juce::MidiMessage(bytes.getData(), numBytes), samplePos);
        }
    }

    inline juce::MemoryBlock buildAudioResult(const juce::AudioBuffer<float>& buffer)
    {
        juce::MemoryOutputStream out;
        out.writeByte((char) MessageType::audioResult);
        out.writeInt(buffer.getNumChannels());
        out.writeInt(buffer.getNumSamples());
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            out.write(buffer.getReadPointer(ch), sizeof(float) * (size_t) buffer.getNumSamples());
        return out.getMemoryBlock();
    }

    inline void parseAudioResult(const juce::MemoryBlock& mb, juce::AudioBuffer<float>& buffer)
    {
        juce::MemoryInputStream in(mb, false);
        in.readByte();
        const int numChannels = in.readInt();
        const int numSamples = in.readInt();
        buffer.setSize(numChannels, numSamples, false, false, true);
        for (int ch = 0; ch < numChannels; ++ch)
            in.read(buffer.getWritePointer(ch), sizeof(float) * (size_t) numSamples);
    }

    inline juce::MemoryBlock buildSimple(MessageType type)
    {
        juce::MemoryOutputStream out;
        out.writeByte((char) type);
        return out.getMemoryBlock();
    }
}
