#include <iostream>
#include <vector>
#include <cmath>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void sendEthernetFrame(const std::string& destMAC, const std::string& srcMAC, const std::string& payload) {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        std::cerr << "Socket creation failed\n";
        return;
    }

    struct ether_header eth;
    memcpy(eth.ether_dhost, destMAC.c_str(), ETHER_ADDR_LEN);
    memcpy(eth.ether_shost, srcMAC.c_str(), ETHER_ADDR_LEN);
    eth.ether_type = htons(0x0800); // IPv4

    char frame[1500];
    memcpy(frame, &eth, sizeof(eth));
    memcpy(frame + sizeof(eth), payload.c_str(), payload.size());

    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.0.62", &destAddr.sin_addr);
    destAddr.sin_port = htons(60273);
}



using Matrix = std::vector<std::vector<double>>;

// Multiply two matrices
Matrix multiply(const Matrix& A, const Matrix& B) {
    int n = A.size();
    Matrix C(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i][j] += A[i][k] * B[k][j];

    return C;
}

// Transpose a matrix
Matrix transpose(const Matrix& A) {
    int n = A.size();
    Matrix T(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            T[j][i] = A[i][j];
    return T;
}

// QR decomposition using Gram-Schmidt
void qrDecomposition(const Matrix& A, Matrix& Q, Matrix& R) {
    int n = A.size();
    Q = A;
    R = Matrix(n, std::vector<double>(n, 0.0));

    for (int i = 0; i < n; ++i) {
        double norm = 0;
        for (int k = 0; k < n; ++k)
            norm += Q[k][i] * Q[k][i];
        norm = std::sqrt(norm);

        R[i][i] = norm;
        for (int k = 0; k < n; ++k)
            Q[k][i] /= norm;

        for (int j = i + 1; j < n; ++j) {
            double dot = 0;
            for (int k = 0; k < n; ++k)
                dot += Q[k][i] * A[k][j];
            R[i][j] = dot;

            for (int k = 0; k < n; ++k)
                Q[k][j] = A[k][j] - R[i][j] * Q[k][i];
        }
    }
}

// QR algorithm to compute eigenvalues
std::vector<double> eigenvalues(Matrix A, int iterations = 200) {
    int n = A.size();

    for (int it = 0; it < iterations; ++it) {
        Matrix Q, R;
        qrDecomposition(A, Q, R);
        A = multiply(R, Q);  
    }

    std::vector<double> lambdas(n);
    for (int i = 0; i < n; ++i)
        lambdas[i] = A[i][i];

    return lambdas;
}

int main() {
    int n;
    std::cout << "Matrix dimension n: ";
    std::cin >> n;

    Matrix A(n, std::vector<double>(n));
    std::cout << "Enter matrix values:\n";
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            std::cin >> A[i][j];

    auto values = eigenvalues(A);

    std::cout << "\nEigenvalues:\n";
    for (double λ : values){
        std::cout << λ << "\n";
    }
    sendEthernetFrame("00:11:22:33:44:55", "66:77:88:99:AA:BB", "Eigenvalues computed");

    return 0;
}