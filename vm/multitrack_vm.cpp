#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// WAV Writer (mesmo da VM anterior)
class SimpleWAVWriter {
public:
    std::vector<int16_t> samples;
    int sample_rate;
    
    SimpleWAVWriter(int rate = 44100) : sample_rate(rate) {}
    
    void addSample(double sample) {
        // Clamp to [-1, 1] and convert to 16-bit
        sample = std::max(-1.0, std::min(1.0, sample));
        int16_t sample_16 = static_cast<int16_t>(sample * 32767);
        samples.push_back(sample_16);
    }
    
    bool writeWAV(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) return false;
        
        // WAV header
        uint32_t file_size = 44 + samples.size() * 2 - 8;
        uint32_t data_size = samples.size() * 2;
        uint16_t audio_format = 1; // PCM
        uint16_t num_channels = 1; // Mono
        uint32_t byte_rate = sample_rate * 2;
        uint16_t block_align = 2;
        uint16_t bits_per_sample = 16;
        
        file.write("RIFF", 4);
        file.write(reinterpret_cast<const char*>(&file_size), 4);
        file.write("WAVE", 4);
        file.write("fmt ", 4);
        
        uint32_t fmt_size = 16;
        file.write(reinterpret_cast<const char*>(&fmt_size), 4);
        file.write(reinterpret_cast<const char*>(&audio_format), 2);
        file.write(reinterpret_cast<const char*>(&num_channels), 2);
        file.write(reinterpret_cast<const char*>(&sample_rate), 4);
        file.write(reinterpret_cast<const char*>(&byte_rate), 4);
        file.write(reinterpret_cast<const char*>(&block_align), 2);
        file.write(reinterpret_cast<const char*>(&bits_per_sample), 2);
        
        file.write("data", 4);
        file.write(reinterpret_cast<const char*>(&data_size), 4);
        file.write(reinterpret_cast<const char*>(samples.data()), data_size);
        
        return true;
    }
};

// Evento musical agendado
struct AudioEvent {
    int timestamp_ticks;
    int track_id;
    std::string instruction;
    
    AudioEvent(int time, int track, std::string instr) 
        : timestamp_ticks(time), track_id(track), instruction(instr) {}
};

// Comparador para ordenar eventos por tempo
struct EventComparator {
    bool operator()(const AudioEvent& a, const AudioEvent& b) {
        return a.timestamp_ticks < b.timestamp_ticks;
    }
};

// Sintetizador para tracks individuais
class TrackSynthesizer {
private:
    int sample_rate;
    
public:
    TrackSynthesizer(int rate = 44100) : sample_rate(rate) {}
    
    // Gera samples para uma nota em um buffer específico
    void renderNote(std::vector<float>& buffer, int start_sample, int midi_note, int velocity, int duration_ticks, int track_type) {
        double frequency = 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
        
        // Convert ticks to samples (480 ticks per beat, 120 BPM)
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        // Ensure buffer is large enough
        if (start_sample + num_samples > buffer.size()) {
            buffer.resize(start_sample + num_samples, 0.0f);
        }
        
        // Generate and mix into buffer
        for (int i = 0; i < num_samples; i++) {
            double t = static_cast<double>(i) / sample_rate;
            double envelope, sample = 0.0;
            double amplitude = (velocity / 127.0) * 0.3;
            
            if (track_type == 0) { // Bass
                envelope = std::exp(-t * 2.0);
                sample = amplitude * envelope * std::sin(2.0 * M_PI * frequency * t);
            } else if (track_type == 1) { // Guitar
                envelope = std::exp(-t * 1.5);
                // Square wave for guitar
                sample = amplitude * envelope * (std::sin(2.0 * M_PI * frequency * t) > 0 ? 1.0 : -1.0) * 0.5;
            }
            
            // Mix into existing buffer (additive synthesis)
            buffer[start_sample + i] += sample;
        }
    }
    
    // Gera samples para um acorde
    void renderChord(std::vector<float>& buffer, int start_sample, const std::vector<int>& midi_notes, int velocity, int duration_ticks) {
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        if (start_sample + num_samples > buffer.size()) {
            buffer.resize(start_sample + num_samples, 0.0f);
        }
        
        for (int i = 0; i < num_samples; i++) {
            double t = static_cast<double>(i) / sample_rate;
            double envelope = std::exp(-t * 1.5);
            double mixed_sample = 0.0;
            
            for (int midi_note : midi_notes) {
                double frequency = 440.0 * std::pow(2.0, (midi_note - 69) / 12.0);
                double amplitude = (velocity / 127.0) * envelope * 0.15;
                mixed_sample += amplitude * std::sin(2.0 * M_PI * frequency * t);
            }
            
            buffer[start_sample + i] += mixed_sample;
        }
    }
    
    // Gera samples para drums
    void renderDrum(std::vector<float>& buffer, int start_sample, int drum_type, int velocity, int duration_ticks) {
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = duration_ticks / ticks_per_second;
        int num_samples = static_cast<int>(duration_seconds * sample_rate);
        
        if (start_sample + num_samples > buffer.size()) {
            buffer.resize(start_sample + num_samples, 0.0f);
        }
        
        for (int i = 0; i < num_samples; i++) {
            double t = static_cast<double>(i) / sample_rate;
            double amplitude = (velocity / 127.0) * 0.4;
            double sample = 0.0;
            
            if (drum_type == 0) { // Kick
                double envelope = std::exp(-t * 15.0);
                double frequency = 60.0 * std::exp(-t * 50.0);
                sample = amplitude * envelope * std::sin(2.0 * M_PI * frequency * t);
            } else if (drum_type == 1) { // Snare
                double envelope = std::exp(-t * 10.0);
                sample = amplitude * envelope * (2.0 * (rand() / double(RAND_MAX)) - 1.0);
            } else if (drum_type == 2) { // Hi-hat
                double envelope = std::exp(-t * 20.0);
                sample = amplitude * envelope * (2.0 * (rand() / double(RAND_MAX)) - 1.0) * 0.5;
            }
            
            buffer[start_sample + i] += sample;
        }
    }
};

// VM Multitrack principal
class MultitrackVM {
private:
    std::vector<AudioEvent> events;
    std::vector<std::vector<float>> track_buffers; // 3 tracks: bass, guitar, drums
    TrackSynthesizer synth;
    int registers[4] = {0};
    std::map<std::string, int> labels;
    int program_counter = 0;
    int current_time_ticks = 0;
    int sample_rate;
    
public:
    MultitrackVM(int rate = 44100) : sample_rate(rate) {
        track_buffers.resize(3); // 3 tracks
    }
    
    // Agenda um evento musical
    void scheduleEvent(int timestamp, int track_id, const std::string& instruction) {
        events.push_back(AudioEvent(timestamp, track_id, instruction));
        std::cout << "  Scheduled at " << timestamp << " ticks: TRACK " << track_id << " " << instruction << std::endl;
    }
    
    // Processa arquivo GBASM e agenda eventos
    bool loadGBASM(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) return false;
        
        std::vector<std::string> instructions;
        std::string line;
        int line_number = 0;
        
        // Primeira passada: coletar instruções e labels
        while (std::getline(file, line)) {
            line_number++;
            
            // Remove comentários
            size_t comment_pos = line.find(';');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;
            
            // Check for labels
            if (line[0] == ':') {
                std::string label = line.substr(1);
                labels[label] = instructions.size();
                std::cout << "Label " << label << " at instruction " << instructions.size() << std::endl;
                continue;
            }
            
            instructions.push_back(line);
        }
        
        // Segunda passada: executar e agendar eventos
        program_counter = 0;
        current_time_ticks = 0;
        
        while (program_counter < instructions.size()) {
            if (!executeInstruction(instructions[program_counter])) {
                break;
            }
            program_counter++;
        }
        
        // Ordenar eventos por tempo
        std::sort(events.begin(), events.end(), EventComparator());
        
        std::cout << "\nScheduled " << events.size() << " audio events" << std::endl;
        return true;
    }
    
    // Executa uma instrução GBASM
    bool executeInstruction(const std::string& instruction) {
        std::istringstream iss(instruction);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "SET_TEMPO" || cmd == "SET_TS") {
            std::cout << "  " << instruction << std::endl;
        } else if (cmd == "TRACK") {
            // TRACK é implícito nos eventos agendados
        } else if (cmd == "NOTE") {
            int pitch, velocity, duration;
            iss >> pitch >> velocity >> duration;
            scheduleEvent(current_time_ticks, 0, instruction); // Bass = track 0
        } else if (cmd == "CHORD") {
            scheduleEvent(current_time_ticks, 1, instruction); // Guitar = track 1
        } else if (cmd == "DRUM") {
            scheduleEvent(current_time_ticks, 2, instruction); // Drums = track 2
        } else if (cmd == "WAIT") {
            int duration;
            iss >> duration;
            current_time_ticks += duration;
            std::cout << "  WAIT " << duration << " (now at " << current_time_ticks << " ticks)" << std::endl;
        } else if (cmd == "LOAD") {
            std::string reg;
            int value;
            iss >> reg >> value;
            int reg_num = reg[1] - '0';
            if (reg_num >= 0 && reg_num < 4) {
                registers[reg_num] = value;
                std::cout << "  LOAD " << reg << " " << value << std::endl;
            }
        } else if (cmd == "DECJNZ") {
            std::string reg, label;
            iss >> reg >> label;
            int reg_num = reg[1] - '0';
            if (reg_num >= 0 && reg_num < 4) {
                registers[reg_num]--;
                std::cout << "  DECJNZ " << reg << " " << label << " (" << reg << "=" << registers[reg_num] << ")" << std::endl;
                if (registers[reg_num] > 0) {
                    if (labels.find(label) != labels.end()) {
                        program_counter = labels[label] - 1; // -1 porque será incrementado
                    }
                }
            }
        } else if (cmd == "JMP") {
            std::string label;
            iss >> label;
            if (labels.find(label) != labels.end()) {
                program_counter = labels[label] - 1;
                std::cout << "  JMP " << label << std::endl;
            }
        } else if (cmd == "HALT") {
            std::cout << "  HALT" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Renderiza todas as tracks baseado nos eventos agendados
    void renderTracks() {
        if (events.empty()) return;
        
        // Calcular duração total necessária
        int max_time = 0;
        for (const auto& event : events) {
            max_time = std::max(max_time, event.timestamp_ticks);
        }
        
        // Adicionar buffer extra para as durações das notas
        max_time += 2000; // 2000 ticks extras
        
        // Convert to samples
        double beats_per_second = 120.0 / 60.0;
        double ticks_per_second = 480.0 * beats_per_second;
        double duration_seconds = max_time / ticks_per_second;
        int total_samples = static_cast<int>(duration_seconds * sample_rate);
        
        // Inicializar buffers das tracks
        for (auto& track : track_buffers) {
            track.assign(total_samples, 0.0f);
        }
        
        std::cout << "\nRendering " << events.size() << " events across " << total_samples << " samples..." << std::endl;
        
        // Renderizar cada evento na track correspondente
        for (const auto& event : events) {
            int start_sample = static_cast<int>((event.timestamp_ticks / ticks_per_second) * sample_rate);
            
            std::istringstream iss(event.instruction);
            std::string cmd;
            iss >> cmd;
            
            if (cmd == "NOTE") {
                int pitch, velocity, duration;
                iss >> pitch >> velocity >> duration;
                synth.renderNote(track_buffers[event.track_id], start_sample, pitch, velocity, duration, event.track_id);
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
                synth.renderChord(track_buffers[event.track_id], start_sample, pitches, velocity, duration);
            } else if (cmd == "DRUM") {
                int drum_type, velocity, duration;
                iss >> drum_type >> velocity >> duration;
                synth.renderDrum(track_buffers[event.track_id], start_sample, drum_type, velocity, duration);
            }
        }
    }
    
    // Mixa todas as tracks em um buffer master
    void mixToWAV(SimpleWAVWriter& writer) {
        if (track_buffers.empty()) return;
        
        // Encontrar o tamanho máximo entre todas as tracks
        size_t max_size = 0;
        for (const auto& track : track_buffers) {
            max_size = std::max(max_size, track.size());
        }
        
        std::cout << "Mixing " << track_buffers.size() << " tracks with " << max_size << " samples each..." << std::endl;
        
        // Mix tracks
        for (size_t i = 0; i < max_size; i++) {
            float mixed_sample = 0.0f;
            
            // Somar todas as tracks
            for (const auto& track : track_buffers) {
                if (i < track.size()) {
                    mixed_sample += track[i];
                }
            }
            
            // Aplicar compressão suave para evitar clipping
            mixed_sample = std::tanh(mixed_sample * 0.7) * 0.8;
            
            writer.addSample(mixed_sample);
        }
    }
    
    // Função principal de execução
    bool execute(const std::string& input_file, const std::string& output_file) {
        std::cout << "Multitrack GarageBand VM" << std::endl;
        std::cout << "Loading: " << input_file << " -> " << output_file << std::endl << std::endl;
        
        if (!loadGBASM(input_file)) {
            std::cerr << "Error loading GBASM file" << std::endl;
            return false;
        }
        
        renderTracks();
        
        SimpleWAVWriter writer(sample_rate);
        mixToWAV(writer);
        
        if (!writer.writeWAV(output_file)) {
            std::cerr << "Error writing WAV file" << std::endl;
            return false;
        }
        
        std::cout << "✓ Multitrack WAV generated: " << output_file << std::endl;
        std::cout << "✓ File size: " << writer.samples.size() * 2 << " bytes" << std::endl;
        
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4 || std::string(argv[2]) != "-o") {
        std::cerr << "Usage: " << argv[0] << " input.gbasm -o output.wav" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = argv[3];
    
    MultitrackVM vm;
    if (!vm.execute(input_file, output_file)) {
        return 1;
    }
    
    return 0;
}
