import random
import os

TECH_WORDS = [
    "algorithm", "data", "structure", "index", "inverted", "hash", "performance", 
    "complexity", "database", "query", "search", "engine", "document", "processor", 
    "memory", "network", "server", "client", "protocol", "encryption", "firewall",
    "software", "hardware", "variable", "function", "compiler", "debugging", 
    "recursion", "optimization", "calculus", "thermodynamics", "quantum", 
    "velocity", "inertia", "gravity", "telescope", "microscope", "laboratory",
    "experiment", "analysis", "synthesis", "theory", "model", "simulation",
    "interface", "system", "architecture", "deployment", "storage", "analytics"
]

POLITIC_WORDS = [
    "government", "policy", "election", "president", "congress", "senate", "law", 
    "justice", "economy", "reform", "taxation", "rights", "democracy", "republic", 
    "monarchy", "parliament", "treaty", "diplomacy", "sanction", "warfare", 
    "revolution", "conflict", "trade", "market", "currency", "inflation", 
    "banking", "budget", "history", "ancient", "medieval", "modern", "empire", 
    "citizen", "security", "protest", "legislation", "capitalism", "socialism"
]

CULTURE_WORDS = [
    "art", "painting", "sculpture", "music", "poetry", "novel", "theater", 
    "emotion", "beauty", "expression", "artist", "history", "philosophy", 
    "ethics", "morality", "existence", "meaning", "consciousness", "perception", 
    "imagination", "fantasy", "dream", "love", "sorrow", "joy", "grief", 
    "mystery", "adventure", "character", "plot", "metaphor", "symbolism", 
    "dramatic", "spiritual", "meditation", "cinema", "literature", "heritage"
]


WORDS_PER_FILE = 10000


def generate_file(filepath, num_words, word_source):
    with open(filepath, 'w', encoding='utf-8') as f:
        for _ in range(num_words):
            word = random.choice(word_source)
            if random.random() < 0.08:
                word += random.choice([".", ",", "!", " "])
            if random.random() < 0.1:
                word = word.capitalize()
            f.write(word + " ")
            if random.random() < 0.05:
                f.write("\n")

def generate_corpus(num_documents, corpus_type, custom_dir_name):
    current_output_dir = custom_dir_name
    
    if not os.path.exists(current_output_dir):
        os.makedirs(current_output_dir)

    print(f"\n--- ГЕНЕРАЦИЯ {num_documents} ДОКУМЕНТОВ ({corpus_type}) в {custom_dir_name} ---")

    for i in range(1, num_documents + 1):
        filename = os.path.join(current_output_dir, f"doc_{i:04}.txt")
        
        if corpus_type == 'uniform':
            source = TECH_WORDS 
        elif corpus_type == 'diverse':
            if i % 3 == 0:
                source = TECH_WORDS
            elif i % 3 == 1:
                source = POLITIC_WORDS
            else:
                source = CULTURE_WORDS
        else:
            raise ValueError("Неверный тип корпуса.")

        generate_file(filename, WORDS_PER_FILE, source)
        if i % 100 == 0 or i == num_documents:
            print(f"Создано файлов: {i}")

    print(f"Генерация завершена в папке: {current_output_dir}")
    return current_output_dir


if __name__ == "__main__":
    
    generate_corpus(10, 'diverse', 'test_docs10')
    
    generate_corpus(100, 'diverse', 'test_docs100')
    
    generate_corpus(1000, 'diverse', 'test_docs1000')

    generate_corpus(5000, 'diverse', 'test_docs5000')

    generate_corpus(10000, 'diverse', 'test_docs10000') 
    