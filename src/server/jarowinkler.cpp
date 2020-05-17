#include "jarowinkler.h"
#include <bits/stdc++.h>
#include <QString>
#include <QVector>



// Function to calculate the
// Jaro Similarity of two strings
double jaroDistance(const QString &s1, const QString &s2)
{
    // If the strings are equal
    if (s1 == s2)
        return 1.0;

    // Length of two strings
    int len1 = s1.length();
    int len2 = s2.length();

    if (len1 == 0 || len2 == 0)
        return 0.0;

    // Maximum distance upto which matching
    // is allowed
    int max_dist = std::floor(std::max(len1, len2) / 2) - 1;

    // Count of matches
    int match = 0;

    // Hash for matches
    QVector<int> hash_s1(s1.length());
    QVector<int> hash_s2(s2.length());

    // Traverse throgh the first string
    for (int i = 0; i < len1; i++)
    {

        // Check if there is any matches
        for (int j = std::max(0, i - max_dist); j < std::min(len2, i + max_dist + 1); j++)
            // If there is a match
            if (s1[i] == s2[j] && hash_s2[j] == 0)
            {
                hash_s1[i] = 1;
                hash_s2[j] = 1;
                match++;
                break;
            }
    }

    // If there is no match
    if (match == 0)
        return 0.0;

    // Number of transpositions
    double t = 0;

    int point = 0;

    // Count number of occurances
    // where two characters match but
    // there is a third matched character
    // in between the indices
    for (int i = 0; i < len1; i++)
        if (hash_s1[i])
        {

            // Find the next matched character
            // in second string
            while (hash_s2[point] == 0)
                point++;

            if (s1[i] != s2[point++])
                t++;
        }

    t /= 2;

    // Return the Jaro Similarity
    return (((double)match) / ((double)len1) +
            ((double)match) / ((double)len2) +
            ((double)match - t) / ((double)match)) /
           3.0;
}

// Jaro Winkler Similarity
double jaroWinkler(const QString &s1, const QString &s2)
{
    double jaro_dist = jaroDistance(s1, s2);

    // If the jaro Similarity is above a threshold
    if (jaro_dist > 0.7)
    {

        // Find the length of common prefix
        int prefix = 0;

        for (int i = 0; i < std::min(s1.length(), s2.length()); i++)
        {
            // If the characters match
            if (s1[i] == s2[i])
                prefix++;

            // Else break
            else
                break;
        }

        // Maximum of 4 characters are allowed in prefix
        prefix = std::min(4, prefix);

        // Calculate jaro winkler Similarity
        jaro_dist += 0.1 * prefix * (1 - jaro_dist);
    }

    return jaro_dist;
}
