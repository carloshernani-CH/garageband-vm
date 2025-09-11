#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdint>

// Simple WAV writer
class SimpleWAVWriter {
private:
    std::vector<int16_t> samples;
    int sample_rate;
    
public:
    SimpleWAVWriter(int rate = 44100) : sample_rate(rate) {}
    
    void addSample(double value) {
        // Clamp and convert to 16-bit
        value = std::max(-1.0, std::min(1.0, value));
        samples.push_back(static_cast<int16_t>(value * 32767));
    }
    
    bool writeToFile(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) return false;
        
        // WAV header
        uint32_t data_size = samples.size() * 2;
        uint32_t file_size = 44 + data_size - 8;
        
        // RIFF header
        file.write("RIFF", 4);
        file.write(reinterpret_cast<const char*>(&file_size), 4);
        file.write("WAVE", 4);
        
        // fmt chunk
        file.write("fmt ", 4);
        uint32_t fmt_size = 16;
        uint16_t audio_format = 1; // PCM
        uint16_t channels = 1;     // Mono
        uint32_t byte_rate = sample_rate * channels * 2;
        uint16_t block_align = channels * 2;
        uint16_t bits_per_sample = 16;
        
        file.write(reinterpret_cast<const char*>(&fmt_size), 4);
        file.write(reinterpret_cast<const char*>(&audio_format), 2);
        file.write(reinterpret_cast<const char*>(&channels), 2);
        file.write(reinterpret_cast<const char*>(&sample_rate), 4);
        file.write(reinterpret_cast<const char*>(&byte_rate), 4);
        file.write(reinterpret_cast<const char*>(&block_align), 2);
        file.write(reinterpret_cast<const char*>(&bits_per_sample), 2);
        
        // data chunk
        file.write("data", 4);
        file.write(reinterpret_cast<const char*>(&data_size), 4);
        file.write(reinterpret_cast<const char*>(samples.data()), data_size);
        
        return true;
    }
};

// Simple synthesizer
class SimpleSynth {
private:
    int sample_rate;
    
public:
    SimpleSynth(int rate = 44100) : sample_rate(rate) {}
    
    void generateNote(SimpleWAVWriter& writer, int midi_note, int velocity, int duration_ticks) {
        // Convert MIDI note to frequency
        double frequency = 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
        
        // Convert ticks to samples (480 ticks per beat, 120 BPM)
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        // Generate sine wave with exponential decay
        for (int i = 0; i < num_samples; i++) {
            double t = static_cast<double>(i) / sample_rate;
            double envelope = std::exp(-t * 2.0); // Exponential decay
            double amplitude = (velocity / 127.0) * envelope * 0.3; // Scale volume
            double sample = amplitude * std::sin(2.0 * M_PI * frequency * t);
            writer.addSample(sample);
        }
    }
    
    void generateChord(SimpleWAVWriter& writer, const std::vector<int>& midi_notes, int velocity, int duration_ticks) {
        // Convert ticks to samples
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        // Generate chord by mixing all notes
        for (int i = 0; i < num_samples; i++) {
            double t = static_cast<double>(i) / sample_rate;
            double envelope = std::exp(-t * 1.5); // Slower decay for chords
            double mixed_sample = 0.0;
            
            // Mix all notes in the chord
            for (int midi_note : midi_notes) {
                double frequency = 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
                double amplitude = (velocity / 127.0) * envelope * 0.15; // Lower volume per note
                mixed_sample += amplitude * std::sin(2.0 * M_PI * frequency * t);
            }
            
            // Normalize to prevent clipping
            mixed_sample = std::max(-1.0, std::min(1.0, mixed_sample));
            writer.addSample(mixed_sample);
        }
    }
    
    void generateDrum(SimpleWAVWriter& writer, int drum_type, int velocity, int duration_ticks) {
        // Convert ticks to samples
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        for (int i = 0; i < num_samples; i++) {
            double t = static_cast<double>(i) / sample_rate;
            double amplitude = (velocity / 127.0) * 0.4;
            double sample = 0.0;
            
            if (drum_type == 0) { // Kick
                double envelope = std::exp(-t * 15.0);
                double frequency = 60.0 * std::exp(-t * 50.0); // Pitch decay
                sample = amplitude * envelope * std::sin(2.0 * M_PI * frequency * t);
            } else if (drum_type == 1) { // Snare
                double envelope = std::exp(-t * 10.0);
                // White noise for snare
                sample = amplitude * envelope * (2.0 * (rand() / double(RAND_MAX)) - 1.0);
            } else if (drum_type == 2) { // Hi-hat
                double envelope = std::exp(-t * 20.0);
                // High frequency noise
                sample = amplitude * envelope * (2.0 * (rand() / double(RAND_MAX)) - 1.0) * 0.5;
            }
            
            writer.addSample(sample);
        }
    }
    
    void generateSilence(SimpleWAVWriter& writer, int duration_ticks) {
        // Convert ticks to samples
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        for (int i = 0; i < num_samples; i++) {
            writer.addSample(0.0);
        }
    }
};

// Simple VM
class SimpleVM {
private:
    int registers[4] = {0}; // R0, R1, R2, R3
    int current_tempo = 120;
    int current_track = 0;
    SimpleSynth synth;
    
public:
    bool executeProgram(const std::string& gbasm_file, const std::string& output_file) {
        std::ifstream file(gbasm_file);
        if (!file) {
            std::cerr << "Erro: não foi possível abrir " << gbasm_file << std::endl;
            return false;
        }
        
        SimpleWAVWriter writer;
        std::string line;
        
        std::cout << "Executando programa GBASM..." << std::endl;
        
        while (std::getline(file, line)) {
            // Remove comments and trim
            size_t comment_pos = line.find(';');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;
            
            if (!executeInstruction(line, writer)) {
                std::cerr << "Erro executando instrução: " << line << std::endl;
                return false;
            }
        }
        
        std::cout << "Gerando arquivo WAV..." << std::endl;
        if (!writer.writeToFile(output_file)) {
            std::cerr << "Erro: não foi possível escrever " << output_file << std::endl;
            return false;
        }
        
        std::cout << "✓ Arquivo WAV gerado: " << output_file << std::endl;
        return true;
    }
    
private:
    bool executeInstruction(const std::string& instruction, SimpleWAVWriter& writer) {
        std::istringstream iss(instruction);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "SET_TEMPO") {
            int tempo;
            iss >> tempo;
            current_tempo = tempo;
            std::cout << "  SET_TEMPO " << tempo << std::endl;
        } else if (cmd == "SET_TS") {
            int num, den;
            iss >> num >> den;
            std::cout << "  SET_TS " << num << "/" << den << std::endl;
        } else if (cmd == "TRACK") {
            int track;
            iss >> track;
            current_track = track;
            std::cout << "  TRACK " << track << std::endl;
        } else if (cmd == "NOTE") {
            int pitch, velocity, duration;
            iss >> pitch >> velocity >> duration;
            std::cout << "  NOTE " << pitch << " " << velocity << " " << duration << std::endl;
            synth.generateNote(writer, pitch, velocity, duration);
        } else if (cmd == "CHORD") {
            int num_notes;
            iss >> num_notes;
            std::vector<int> pitches;
            for (int i = 0; i < num_notes; i++) {
                int pitch;
                iss >> pitch;
                pitches.push_back(pitch);
            }
            int velocity, duration;
            iss >> velocity >> duration;
            std::cout << "  CHORD " << num_notes << " notes, vel " << velocity << ", dur " << duration << std::endl;
            synth.generateChord(writer, pitches, velocity, duration);
        } else if (cmd == "DRUM") {
            int drum_type, velocity, duration;
            iss >> drum_type >> velocity >> duration;
            std::cout << "  DRUM " << drum_type << " " << velocity << " " << duration << std::endl;
            synth.generateDrum(writer, drum_type, velocity, duration);
        } else if (cmd == "WAIT") {
            int duration;
            iss >> duration;
            std::cout << "  WAIT " << duration << std::endl;
            synth.generateSilence(writer, duration);
        } else if (cmd == "LOAD") {
            std::string reg;
            int value;
            iss >> reg >> value;
            int reg_num = reg[1] - '0'; // R0 -> 0, R1 -> 1, etc.
            if (reg_num >= 0 && reg_num < 4) {
                registers[reg_num] = value;
            }
            std::cout << "  LOAD " << reg << " " << value << std::endl;
        } else if (cmd == "DECJNZ") {
            std::string reg, label;
            iss >> reg >> label;
            int reg_num = reg[1] - '0';
            if (reg_num >= 0 && reg_num < 4) {
                registers[reg_num]--;
                std::cout << "  DECJNZ " << reg << " " << label << " (R" << reg_num << "=" << registers[reg_num] << ")" << std::endl;
                // Note: Jump logic would need more complex instruction pointer management
            }
        } else if (cmd == "JMP") {
            std::string label;
            iss >> label;
            std::cout << "  JMP " << label << " (jump instruction)" << std::endl;
        } else if (cmd == "JZ") {
            std::string reg, label;
            iss >> reg >> label;
            std::cout << "  JZ " << reg << " " << label << " (conditional jump)" << std::endl;
        } else if (cmd == "HALT") {
            std::cout << "  HALT" << std::endl;
            return true;
        } else if (cmd[0] == ':') {
            // Label - ignore for now
            std::cout << "  " << cmd << " (label)" << std::endl;
        } else {
            std::cout << "  " << cmd << " (unknown instruction)" << std::endl;
        }
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4 || std::string(argv[2]) != "-o") {
        std::cout << "Uso: " << argv[0] << " <input.gbasm> -o <output.wav>" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[3];
    
    std::cout << "GarageBand VM - Interpretador GBASM" << std::endl;
    std::cout << "Executando: " << input_file << " -> " << output_file << std::endl;
    std::cout << std::endl;
    
    SimpleVM vm;
    if (!vm.executeProgram(input_file, output_file)) {
        return 1;
    }
    
    std::cout << std::endl << "✓ Execução concluída com sucesso!" << std::endl;
    return 0;
}
